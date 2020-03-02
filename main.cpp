#include <iostream>
#include "swosgl.h"

#ifdef IMGUI_OVERLAY
#include "examples/imgui_impl_sdl.h"
#endif

int kVgaWidth = 480;
int kVgaHeight = 272;

int m_windowWidth = kVgaWidth * 2;
int m_windowHeight = kVgaHeight * 2;

SDL_Window* m_window;
SDL_Renderer* m_renderer;
SDL_GLContext m_context;

int gRenderMode = RM_OpenGL;
std::string shname = "crt-geom";

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
	if (argc > 1)
		shname = std::string(argv[1]);

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