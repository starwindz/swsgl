#pragma once
#include <GL/glew.h>

class GLTexture
{
public:
  GLTexture();
  virtual ~GLTexture();
  void Generate(GLuint width, GLuint height, GLuint* data);
  GLuint GetHeight() const { return m_Height; }
  GLuint GetWidth() const { return m_Width; }
  GLuint GetInternalFormat() const { return m_InternalFormat; }
  void SetInternalFormat(GLuint format) { m_InternalFormat = format; }
  GLuint GetImageFormat() const { return m_ImageFormat; }
  void SetImageFormat(GLuint format) { m_ImageFormat = format; }
  void SetFilterFormat(GLuint min, GLuint mag);
  void SetWrapMode(GLuint wrapS, GLuint wrapT);
  void Bind() const;
  void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
  void Update(GLuint* data);
  GLuint GetTextureID() { return m_ID; }
private:
  GLuint m_ID;
  GLuint m_Width;
  GLuint m_Height;
  GLuint m_InternalFormat;
  GLuint m_ImageFormat;
  GLuint m_WrapMode_S;
  GLuint m_WrapMode_T;
  GLuint m_FilterMax;
  GLuint m_FilterMin;
};

