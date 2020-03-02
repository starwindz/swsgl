#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLTexture.h"
#include "GLShader.h"
#include <memory>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoords;
	Vertex(glm::vec3 pos, glm::vec2 texcoords)
		: Position(pos), TexCoords(texcoords)
	{}
	Vertex(float x, float y, float z, float u, float v)
		: Position(glm::vec3(x,y,z)), TexCoords(glm::vec2(u,v))
	{}
};

class GLMesh
{
public:
	GLMesh();
	virtual ~GLMesh();
	void SetupMesh(std::vector<Vertex>& vertices, std::vector<GLuint> indices, std::shared_ptr<GLTexture> texture);
	void SetColor(glm::vec4 col) { m_Color = col; }
	void SetPosition(glm::vec3 pos) { m_Position = pos; }
	void SetSize(glm::vec3 size) { m_Size = size;  }
	void Draw(std::shared_ptr<GLShader> shader, glm::mat4 projection);
	void DrawFrameBuffer(std::shared_ptr<GLShader> shader);
private:
	std::vector<Vertex> m_Vertices;
	std::vector<GLuint> m_Indices;
	std::shared_ptr<GLTexture> m_Texture;
	glm::vec4 m_Color;
	glm::vec3 m_Position;
	glm::vec3 m_Size;
	GLuint m_VAO;
};

