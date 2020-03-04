#include "GLPostProcess.h"
#include <gl/glew.h>

std::vector<std::shared_ptr<GLFramebuffer>> GLPostProcess::s_Framebuffers;
std::vector<GLPhase> GLPostProcess::s_Phases;

void GLPostProcess::Init()
{
	s_Framebuffers.clear();
	s_Phases.clear();

}

void GLPostProcess::Begin(const std::vector<GLPhase>& phases)
{
	s_Phases = phases;
	s_Framebuffers.clear();
	for (auto phase : phases)
	{
		std::shared_ptr<GLFramebuffer> buf = std::make_shared<GLFramebuffer>(
			phase.SourceSize.x, phase.SourceSize.y, phase.TargetSize.x, phase.TargetSize.y, 8, phase.Filters);
		s_Framebuffers.push_back(buf);
	}
	s_Framebuffers[0]->Bind(phases[0].Shader);
	glViewport(0, 0, s_Framebuffers[0]->GetWidht(), s_Framebuffers[0]->GetHeight());
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLPostProcess::End(GLuint widht, GLuint height)
{
	if (s_Framebuffers.size() > 1)
	{
		for (int f = 1; f < s_Framebuffers.size(); f++)
		{
			s_Framebuffers[f - 1]->Read();
			s_Framebuffers[f]->Bind(s_Phases[f].Shader);
			glViewport(0, 0, s_Framebuffers[f]->GetWidht(), s_Framebuffers[f]->GetHeight());
			s_Framebuffers[f - 1]->Draw(s_Framebuffers[f - 1]->GetTargetWidht(), s_Framebuffers[f - 1]->GetTargetHeight());
		}
	}
	s_Framebuffers[s_Framebuffers.size() - 1]->Read();
	s_Framebuffers[s_Framebuffers.size() - 1]->UnBind();
	glViewport(0, 0, widht, height);
	glClear(GL_COLOR_BUFFER_BIT);
	s_Phases[s_Phases.size() - 1].Shader->Use();
	s_Framebuffers[s_Framebuffers.size() - 1]->Draw(widht, height);
}
