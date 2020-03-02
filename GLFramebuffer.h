#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "GLTexture.h"
#include "GLShader.h"

struct FrameVertex
{
	glm::vec4 Position;
	glm::vec2 TexCoords;
	FrameVertex(glm::vec4 pos, glm::vec2 texcoords)
		: Position(pos), TexCoords(texcoords)
	{}
	FrameVertex(float x, float y, float z, float w, float u, float v)
		: Position(glm::vec4(x, y, z, w)), TexCoords(glm::vec2(u, v))
	{}
};

class GLFramebuffer
{
public:
	GLFramebuffer(std::shared_ptr<GLShader> shader, GLuint width, GLuint height, GLuint targetwidth, GLuint targetheight, GLuint multisample);
	virtual ~GLFramebuffer() = default;
	void Bind();
	void UnBind();
	GLuint GetWidht() { return m_Width; }
	GLuint GetHeight() { return m_Height; }
	GLuint GetTargetWidht() { return m_TargetWidth; }
	GLuint GetTargetHeight() { return m_TargetHeight; }
	std::shared_ptr<GLTexture> GetTexture() { return m_Texture; }
	GLuint GetID() { return m_FBO; }
	void Draw(GLuint screen_x, GLuint screen_y);
private:
	GLuint m_VAO;
	GLuint m_FBO;
	GLuint m_RBO;
	GLuint m_MSFBO;
	GLuint m_Width;
	GLuint m_Height;
	GLuint m_TargetWidth;
	GLuint m_TargetHeight;
	std::shared_ptr<GLShader> m_Shader;
	std::shared_ptr<GLTexture> m_Texture;
	std::vector<FrameVertex> m_Vertices;
};

