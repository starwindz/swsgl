#include "GLFramebuffer.h"
#include <iostream>



GLFramebuffer::GLFramebuffer(
  GLuint width, GLuint height, GLuint targetwidth, GLuint targetheight, GLuint multisample, GLuint Filter)
  : m_Width(width), m_Height(height), m_TargetWidth(targetwidth), m_TargetHeight(targetheight)
{
  m_Texture = std::make_shared<GLTexture>();
  m_Vertices = {
    FrameVertex(-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
    FrameVertex(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f),
    FrameVertex(-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f),
    FrameVertex(-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
    FrameVertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
    FrameVertex(1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f),
  };
  glGenFramebuffers(1, &m_FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
  m_Texture->SetImageFormat(GL_RGB);
  m_Texture->SetInternalFormat(GL_RGB);
  m_Texture->Bind();
  m_Texture->Generate(m_Width, m_Height, NULL);
  m_Texture->SetFilterFormat(Filter, Filter);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetTextureID(), 0); // Attach texture to framebuffer as its color attachment
  

  glGenRenderbuffers(1, &m_RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, targetwidth, targetheight); // use a single renderbuffer object for both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO); // now actually attach it

  //GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  //glDrawBuffers(1, DrawBuffers);

  m_Texture->Unbind();
  if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE_EXT)
    std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLFramebuffer::~GLFramebuffer()
{
  glDeleteRenderbuffers(1, &m_RBO);
  glDeleteFramebuffers(1, &m_FBO);
  glDeleteVertexArrays(1, &m_VAO);
}

void GLFramebuffer::Bind(std::shared_ptr<GLShader> shader)
{
  m_Shader = shader;
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
}

void GLFramebuffer::Read()
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
}

void GLFramebuffer::UnBind()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Binds both READ and WRITE framebuffer to default framebuffer
  //glDisable(GL_DEPTH_TEST);

}

void GLFramebuffer::Draw(GLuint screen_x, GLuint screen_y)
{
  m_Shader->Use();
  GLuint VBO;
  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(FrameVertex), &m_Vertices[0], GL_STATIC_DRAW);
  GLint locationPosition = glGetAttribLocation(m_Shader->GetID(), "position");
  glEnableVertexAttribArray(locationPosition);
  glVertexAttribPointer(locationPosition, 4, GL_FLOAT, GL_FALSE, sizeof(FrameVertex), 0);
  GLint texturecoord = glGetAttribLocation(m_Shader->GetID(), "texCoord");
  glEnableVertexAttribArray(texturecoord);
  glVertexAttribPointer(texturecoord, 2, GL_FLOAT, GL_FALSE, sizeof(FrameVertex), (void*)offsetof(FrameVertex, TexCoords));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  m_Shader->Use();
  m_Shader->SetInt("source[0]", 0);
  m_Shader->SetVector4f("targetSize", glm::vec4(m_TargetWidth, m_TargetHeight, 1.0f / m_TargetWidth, 1.0f / m_TargetHeight));
  m_Shader->SetVector4f("outputSize", glm::vec4(m_TargetWidth, m_TargetHeight, 1.0f / m_TargetWidth, 1.0f / m_TargetHeight));
  m_Shader->SetVector4f("sourceSize[0]", glm::vec4(m_Width, m_Height, 1.0f / m_Width, 1.0f / m_Height));
  glActiveTexture(GL_TEXTURE0);
  m_Texture->Bind();
  // draw mesh
  glBindVertexArray(m_VAO);
  glDrawArrays(GL_TRIANGLES,0,6);

  glBindVertexArray(0);

  // always good practice to set everything back to defaults once configured.
  glBindTexture(GL_TEXTURE_2D, 0); 
  glActiveTexture(0);
  m_Shader->UnBind();
}
