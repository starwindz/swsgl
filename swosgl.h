#pragma once
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <memory>
#include <unordered_map>
#include "GLShader.h"
#include "GLTexture.h"
#include "GLMesh.h"
#include "GLFramebuffer.h"
#include "GLPostProcess.h"
#ifdef IMGUI_OVERLAY
#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_sdl.h"
#endif

extern int gVgaWidth;
extern int gVgaHeight;
extern int m_windowWidth;
extern int m_windowHeight;

enum RenderMode
{
	RM_SDL,
	RM_OpenGL
};

class SWOSGLRenderer
{
public:
	static SWOSGLRenderer& Get();
	bool CreateWindow(int w, int h);
	void Initialize();
	void Finalize();
	void WindowResize(int w, int h);
	void SwitchWindowedFullscreen();
	void SetUsingShader(const std::string& shadername) { m_UsingShaderName = shadername; }
	void UpdateMenu();
	void RenderMenu();
	void UpdateGame();
	void RenderGame();
	// Shaders
	std::shared_ptr<GLShader> AddShader(const std::string& name, const std::string& vs_fname, const std::string& fs_fname);
	std::shared_ptr<GLShader> GetShader(const std::string& name) { return m_Shaders.at(name); }
	// Textures
	std::shared_ptr<GLTexture> AddTexture(const std::string& name);
	std::shared_ptr<GLTexture> AddTextureFromFile(const std::string& name, const std::string& filename);
	std::shared_ptr<GLTexture> GetTexture(const std::string& name) { return m_Textures.at(name); }
	void AddMenuTextureFromBackgroundImage(const char* texName);
	// Meshes
	std::shared_ptr<GLMesh> AddMesh(const std::string& name);
	std::shared_ptr<GLMesh> GetMesh(const std::string& name) { return m_Meshes.at(name); }
	// Utility functions
	Uint32 setRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	void getRGBA(Uint32 color, Uint8* r, Uint8* g, Uint8* b, Uint8* a);
	void updateTestMenuPixels(Uint32* pixels);
	
private:
	int m_WindowWidth;
	int m_WindowHeight;
	int m_RenderMode;
	SDL_Window* m_Window;
	SDL_GLContext m_Context;
	SDL_Renderer* m_Renderer;
	std::unordered_map<std::string, std::shared_ptr<GLShader>> m_Shaders;
	std::unordered_map<std::string, std::shared_ptr<GLTexture>> m_Textures;
	std::unordered_map<std::string, std::shared_ptr<GLMesh>> m_Meshes;
	std::shared_ptr<GLFramebuffer> m_Framebuffer;
	glm::mat4 m_Projection;
	glm::mat4 m_View;
	bool m_FullScreenDesktop = false;
	std::string m_UsingShaderName = "crt-geom";
private:
	SWOSGLRenderer() = default;
	virtual ~SWOSGLRenderer() = default;
	SWOSGLRenderer(SWOSGLRenderer const&) = delete;
	void operator=(SWOSGLRenderer const&) = delete;
	static SWOSGLRenderer* s_Instance;
	float m_Opacity = 1.0f;
};

