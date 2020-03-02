#include "GLTexture.h"

GLTexture::GLTexture()
  : m_Width(0), m_Height(0), m_InternalFormat(GL_RGBA), m_ImageFormat(GL_RGBA), m_WrapMode_S(GL_CLAMP_TO_EDGE),
  m_WrapMode_T(GL_CLAMP_TO_EDGE), m_FilterMin(GL_NEAREST), m_FilterMax(GL_NEAREST)
{
}

GLTexture::~GLTexture()
{
  if (m_ID != 0)
    glDeleteTextures(1, &m_ID);
}

void GLTexture::Generate(GLuint width, GLuint height, GLuint* data)
{
  m_Width = width;
  m_Height = height;
  glGenTextures(1, &m_ID);
  glBindTexture(GL_TEXTURE_2D, m_ID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, width, height, 0, m_ImageFormat, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapMode_S);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapMode_T);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_FilterMin);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_FilterMax);

  glBindTexture(GL_TEXTURE_2D, 0);
}


void GLTexture::SetFilterFormat(GLuint min, GLuint mag)
{
  m_FilterMax = mag;
  m_FilterMin = min;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_FilterMin);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_FilterMax);
}

void GLTexture::SetWrapMode(GLuint wrapS, GLuint wrapT)
{
  m_WrapMode_S = wrapS;
  m_WrapMode_T = wrapT;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_FilterMin);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_FilterMax);
}

void GLTexture::Bind() const
{
  glBindTexture(GL_TEXTURE_2D, m_ID);
}

void GLTexture::Update(GLuint* data)
{
  if (m_ID != 0)
  {
    Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    Unbind();
  }
}
