/*
	To-do-list

	1. .vs, .fs 파일을 shaders 폴더로 이동하고, 작동 test
	2. gap(Y)에 대한 부분 일반화 확인
	3. AddTexture 메모리 할당 초기화 추가

	4. MonitorID(DisplayID) 관련 option 추가
	5. Fullscreen mode 기능 확인 (F12 key 눌렀을 때 작동하는 code 확인)

	6. CRT-Geom-Halation 예제로 test (BSNES 관련 파일을 anoxic에게 공유)
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