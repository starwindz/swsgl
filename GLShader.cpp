#include "GLShader.h"
#include <iostream>

GLShader::GLShader()
{
}

GLShader::~GLShader()
{
}

void GLShader::LoadShaderFromCode(const std::string& vertexCode, const std::string& fragmentCode)
{
  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();
  // 2. compile shaders
  unsigned int vertex, fragment;
  // vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");
  // fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");
  // shader Program
  m_ID = glCreateProgram();
  glAttachShader(m_ID, vertex);
  glAttachShader(m_ID, fragment);
  glLinkProgram(m_ID);
  checkCompileErrors(m_ID, "PROGRAM");
  // delete the shaders as they're linked into our program now and no longer necessary
  glDetachShader(m_ID, vertex);
  glDetachShader(m_ID, fragment);
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void GLShader::Use()
{
  glUseProgram(m_ID);
}

void GLShader::UnBind()
{
  glUseProgram(0);
}

void GLShader::checkCompileErrors(unsigned int shader, std::string type)
{
  int success;
  char infoLog[1024];
  if (type != "PROGRAM")
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cerr << "[Shader] :: SHADER_COMPILATION_ERROR of type: " << type << "\n";
      std::cerr << "[Shader] :: Log: " <<  infoLog << "\n";
      return;
    }
    std::cout << "[Shader]:: Compilation. OK.\n";
  }
  else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cerr << "[Shader] :: PROGRAM_LINKING_ERROR of type: " << type << "\n";
      std::cerr << "[Shader] :: Log: " << infoLog << "\n";
      return;
    }
    std::cout << "[Shader]:: Linking. OK. Program ID: " << m_ID << "\n";
  }
}

GLint GLShader::GetUniformLocation(const std::string& name) const
{
  if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    return m_UniformLocationCache.at(name);

  GLuint location = glGetUniformLocation(m_ID, name.c_str());
  m_UniformLocationCache[name] = location;
  return location;
}
