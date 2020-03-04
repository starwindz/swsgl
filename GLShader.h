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
  GLint GetUniformLocation(const std::string& name) const;
  void SetBool(const std::string& name, bool value) const { glUniform1i(GetUniformLocation(name), (int)value); }
  void SetInt(const std::string& name, int value) const { glUniform1i(GetUniformLocation(name), value); }
  void SetFloat(const std::string& name, float value) const { glUniform1f(GetUniformLocation(name), value); }
  void SetVector2f(const std::string& name, GLfloat x, GLfloat y) { glUniform2f(GetUniformLocation(name), x, y); }
  void SetVector2f(const std::string& name, const glm::vec2& value) { glUniform2f(GetUniformLocation(name), value.x, value.y); }
  void SetVector3f(const std::string& name, GLfloat x, GLfloat y, GLfloat z) { glUniform3f(GetUniformLocation(name), x, y, z); }
  void SetVector3f(const std::string& name, const glm::vec3& value) { glUniform3f(GetUniformLocation(name), value.x, value.y, value.z); }
  void SetVector4f(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) { glUniform4f(GetUniformLocation(name), x, y, z, w); }
  void SetVector4f(const std::string& name, const glm::vec4& value) { glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w); }
  void SetMatrix4(const std::string& name, const glm::mat4& matrix) { glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix)); }
private:
  mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;
	void checkCompileErrors(unsigned int shader, std::string type);
	GLuint m_ID;
};

