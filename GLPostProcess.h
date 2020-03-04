#pragma once
#include <vector>
#include "GLShader.h"
#include "GLTexture.h"
#include "GLFramebuffer.h"
#include <glm/glm.hpp>
#include <memory>

struct GLPhase
{
	glm::vec2 SourceSize;
	glm::vec2 TargetSize;
	std::shared_ptr<GLShader> Shader;
	GLuint Filters;
	GLPhase(std::shared_ptr<GLShader> shader, glm::vec2 ssize, glm::vec2 tsize, GLuint filter)
		: Shader(shader), SourceSize(ssize), TargetSize(tsize), Filters(filter)
	{}
};

class GLPostProcess
{
public:
	static void Init();
	static void Begin(const std::vector<GLPhase>& phases);
	static void End(GLuint widht, GLuint height);
private:
	static std::vector<std::shared_ptr<GLFramebuffer>> s_Framebuffers;
	static std::vector<GLPhase> s_Phases;
};

