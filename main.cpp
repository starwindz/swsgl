/*
	To-do-list

	1. .vs, .fs ������ shaders ������ �̵��ϰ�, �۵� test
	2. gap(Y)�� ���� �κ� �Ϲ�ȭ Ȯ��
	3. AddTexture �޸� �Ҵ� �ʱ�ȭ �߰�

	4. MonitorID(DisplayID) ���� option �߰�
	5. Fullscreen mode ��� Ȯ�� (F12 key ������ �� �۵��ϴ� code Ȯ��)

	6. CRT-Geom-Halation ������ test (BSNES ���� ������ anoxic���� ����)
*/

#include <iostream>
#include "swosgl.h"

#ifdef IMGUI_OVERLAY
#include "examples/imgui_impl_sdl.h"
#endif

int gVgaWidth = 480;
int gVgaHeight = 270;

int m_windowWidth = gVgaWidth * 2;
int m_windowHeight = gVgaHeight * 2;

SDL_Window* m_window;
SDL_Renderer* m_renderer;
SDL_GLContext m_context;

void initRenderingOpenGL()
{
	SWOSGLRenderer::Get().CreateWindow(m_windowWidth, m_windowHeight);
	SWOSGLRenderer::Get().SetUsingShader(shname);
	SWOSGLRenderer::Get().Initialize();
}

void updateMenuScreenOpenGL()
{
	SWOSGLRenderer::Get().UpdateMenu();
	SWOSGLRenderer::Get().RenderMenu();
}

void updateGameScreenOpenGL()
{
	SWOSGLRenderer::Get().UpdateGame();
	SWOSGLRenderer::Get().RenderGame();
}

void finishRendering()
{
	SWOSGLRenderer::Get().Finalize();
}

int main(int argc, char** argv)
{
	atexit(&finishRendering);

	initRenderingOpenGL();
	
	bool quit = false;
	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
#ifdef IMGUI_OVERLAY
			ImGui_ImplSDL2_ProcessEvent(&e);
#endif
			if (e.type == SDL_QUIT)
				quit = true;
			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_F12)
				{
					SWOSGLRenderer::Get().SwitchWindowedFullscreen();
				}
			}
			if (e.type == SDL_WINDOWEVENT)
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					SWOSGLRenderer::Get().WindowResize(e.window.data1, e.window.data2);
				}
			}

		}
#if (1)
		updateMenuScreenOpenGL();
#else
		updateGameScreenOpenGL();
#endif
	}
	return 0;
}