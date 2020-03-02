#include "GLMesh.h"

GLMesh::GLMesh()
  : m_Color(glm::vec4(1.f)), m_Position(glm::vec3(1.f)), m_Size(glm::vec3(1.f))
{
  m_Indices.clear();
  m_Vertices.clear();
  m_VAO = 0;
}

GLMesh::~GLMesh()
{
}

void GLMesh::SetupMesh(std::vector<Vertex>& vertices, std::vector<GLuint> indices, std::shared_ptr<GLTexture> texture)
{
  m_Indices = indices;
  m_Vertices = vertices;
  m_Texture = texture;
  m_Color = glm::vec4(1.f, 1.f, 1.f, 1.f);
  // create buffers/arrays
  GLuint VBO, EBO;
  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(m_VAO);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // A great thing about structs is that their memory layout is sequential for all its items.
  // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
  // again translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  // set the vertex attribute pointers
  // vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  // vertex normals
  //glEnableVertexAttribArray(1);
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
  // vertex texture coords
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

  glBindVertexArray(0);
}

void GLMesh::Draw(std::shared_ptr<GLShader> shader, glm::mat4 projection)
{
  shader->Use();
  shader->SetMatrix4("projection", projection);
  //
  glm::mat4 model = glm::mat4(1.f); //LoadIdentity
  model = glm::translate(model, m_Position);
  model = glm::scale(model, m_Size);
  shader->SetMatrix4("model", model);
  //
  shader->SetInt("image", 0);
  shader->SetVector4f("color", m_Color);
  glActiveTexture(GL_TEXTURE0);
  m_Texture->Bind();
  // draw mesh
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // always good practice to set everything back to defaults once configured.
  m_Texture->Unbind();
  shader->UnBind();
}

void GLMesh::DrawFrameBuffer(std::shared_ptr<GLShader> shader)
{
  shader->Use();
  glActiveTexture(GL_TEXTURE0);
  m_Texture->Bind();
  // draw mesh
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // always good practice to set everything back to defaults once configured.
  m_Texture->Unbind();
  shader->UnBind();
}
