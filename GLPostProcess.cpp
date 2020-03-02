#include "GLPostProcess.h"
#include <gl/glew.h>

std::vector<std::shared_ptr<GLFramebuffer>> GLPostProcess::s_Framebuffers;

void GLPostProcess::Init(const std::vector<GLPhase>& m_Phases)
{
	s_Framebuffers.clear();
	for (auto phase : m_Phases)
	{
		std::shared_ptr<GLFramebuffer> buf = std::make_shared<GLFramebuffer>(phase.Shader, 
			phase.SourceSize.x, phase.SourceSize.y, phase.TargetSize.x, phase.TargetSize.y ,8);
		s_Framebuffers.push_back(buf);
	}
}

void GLPostProcess::Begin()
{
	s_Framebuffers[0]->Bind();
	glViewport(0, 0, s_Framebuffers[0]->GetWidht(), s_Framebuffers[0]->GetHeight());
}

void GLPostProcess::End(GLuint widht, GLuint height)
{
	if (s_Framebuffers.size() > 1)
	{
		for (int f = 1; f < s_Framebuffers.size(); f++)
		{
			s_Framebuffers[f - 1]->UnBind();
			s_Framebuffers[f]->Bind();
			glViewport(0, 0, s_Framebuffers[f]->GetWidht(), s_Framebuffers[f]->GetHeight());
			s_Framebuffers[f - 1]->Draw(s_Framebuffers[f - 1]->GetTargetWidht(), s_Framebuffers[f - 1]->GetTargetHeight());
		}
	}
	s_Framebuffers[s_Framebuffers.size() - 1]->UnBind();
	glViewport(0, 0, widht, height);
	s_Framebuffers[s_Framebuffers.size() - 1]->Draw(widht, height);
}
