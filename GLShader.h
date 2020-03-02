#pragma once
#include <string>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

class GLShader
{
public:
	GLShader();
	virtual ~GLShader();
	void LoadShaderFromCode(const std::string& vertexCode, const std::string& fragmentCode);
	void Use();
	void UnBind();
  GLuint GetID() { return m_ID; }
  void SetBool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value); }
  void SetInt(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value); }
  void SetFloat(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value); }
  void SetVector2f(const std::string& name, GLfloat x, GLfloat y) { glUniform2f(glGetUniformLocation(m_ID, name.c_str()), x, y); }
  void SetVector2f(const std::string& name, const glm::vec2& value) { glUniform2f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y); }
  void SetVector3f(const std::string& name, GLfloat x, GLfloat y, GLfloat z) { glUniform3f(glGetUniformLocation(m_ID, name.c_str()), x, y, z); }
  void SetVector3f(const std::string& name, const glm::vec3& value) { glUniform3f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y, value.z); }
  void SetVector4f(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) { glUniform4f(glGetUniformLocation(m_ID, name.c_str()), x, y, z, w); }
  void SetVector4f(const std::string& name, const glm::vec4& value) { glUniform4f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y, value.z, value.w); }
  void SetMatrix4(const std::string& name, const glm::mat4& matrix) { glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix)); }
private:
  mutable std::unordered_map<std::string, GLuint> m_UniformLocationCache;
	void checkCompileErrors(unsigned int shader, std::string type);
  GLuint GetUniformLocation(const std::string& name) const;
	GLuint m_ID;
};

