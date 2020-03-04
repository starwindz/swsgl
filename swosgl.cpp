#include "swosgl.h"
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int exists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

SWOSGLRenderer* SWOSGLRenderer::s_Instance;

SWOSGLRenderer& SWOSGLRenderer::Get()
{
	if (s_Instance)
	{
		return *s_Instance;
	}
	else
	{
		s_Instance = new SWOSGLRenderer();
		return *s_Instance;
	}
}

bool SWOSGLRenderer::CreateWindow(int w, int h)
{
	// Init SDL_OPENGL
	std::cout << "Render mode OpenGL.\n";

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cerr << "[ERROR]:: Failed to init SDL.\n";
		return false;
	}

	// Use OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// Create Window
	int wflags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;

#ifdef SWOS_2020
	if (getWindowMode() == 2) {
		flags |= SDL_WINDOW_FULLSCREEN; // If you will to got normal fullscreen
	}
#endif

	m_WindowWidth = w;
	m_WindowHeight = h;

	uint8_t gap = 0;

#ifdef SWOS_2020
	gDisplayID = getMonitorID();
#endif

	m_Window = SDL_CreateWindow(
#ifdef SWOS_2020
		"SWOS 2020",
#else
		"OpenGL Rendering and Shader Test for SWOS 2020",
#endif
		SDL_WINDOWPOS_CENTERED_DISPLAY(gDisplayID), SDL_WINDOWPOS_CENTERED_DISPLAY(gDisplayID),
		m_WindowWidth, m_WindowHeight, wflags
	);
	if (!m_Window) {
		std::cerr << "[ERROR]:: Failed to create Window. " << SDL_GetError() << std::endl;
		return false;
	}

	// Set Render Driver
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

	// Create OpenGL Context
	m_Context = SDL_GL_CreateContext(m_Window);
	SDL_GL_MakeCurrent(m_Window, m_Context);
	
#ifdef SWOS_2020	
	SDL_GL_SetSwapInterval(0);
#else
	SDL_GL_SetSwapInterval(1);
#endif

	// Clear screen
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
	SDL_GL_SwapWindow(m_Window);

	// Initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();
	if (err != GL_NO_ERROR)	{
		std::cerr << "[ERROR]:: Failed to initialize GLEW. " << glewGetErrorString(err) << "\n";
		return false;
	}

#ifdef IMGUI_OVERLAY
	const char* glsl_version = "#version 150";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(m_Window, m_Context);
	ImGui_ImplOpenGL3_Init(glsl_version);
#endif

	// Enable alpha blending mode
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DITHER);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::cout << "Created Window with " << m_WindowWidth << "x" << m_WindowHeight << " size. RenderMode: OpenGL\n";

	m_ShaderMode = SM_CRT_SIMPLE;

	return true;
}

void SWOSGLRenderer::AddMenuTextureFromBackgroundImage(const char *texName)
{
#ifdef SWOS_2020
	if (getMenuBackgroundImage() == 0) {
		if (getGameStyle() == 0) {
			AddTextureFromFile(texName, "swtitle-bg-pc-small.bmp");
		}
		else {
			AddTextureFromFile(texName, "swtitle-bg-amiga-small.bmp");
		}
	}
	else if (getMenuBackgroundImage() == 1) {
		AddTextureFromFile(texName, "swtitle-bg-pc-small.bmp");
	}
	else if (getMenuBackgroundImage() == 2) {
		AddTextureFromFile(texName, "swtitle-bg-amiga-small.bmp");
	}
	else if (getMenuBackgroundImage() == 4) {
		srand((unsigned int)time(NULL));
		int nMin = 0, nMax = 99;
		int randNumber = rand() % (nMax - nMin + 1) + nMin;
		if (randNumber <= 50) {
			AddTextureFromFile(texName, "swtitle-bg-pc-small.bmp");
		}
		else if (randNumber > 50 && randNumber <= 99) {
			AddTextureFromFile(texName, "swtitle-bg-amiga-small.bmp");
		}
	}
#else
	AddTextureFromFile(texName, "swtitle-bg-pc-small.bmp");
#endif
}

void SWOSGLRenderer::Initialize()
{
	// Load Shaders
	AddShader("basic", "sprite.vs", "sprite.fs");

	AddShader("crt-geom", "crt-geom.vs", "crt-geom.fs");
	//AddShader("crt-geom2", "crtgeom.vs", "crtgeom.fs");
	AddShader("gaussian-horiz", "def.vs", "gaussian-horiz.fs");
	AddShader("gaussian-vert", "def.vs", "gaussian-vert.fs");
	AddShader("combine", "def.vs", "combine.fs");

	AddShader("none", "def.vs", "def.fs");
	AddShader("crt-simple", "crt-simple.vs", "crt-simple.fs");
	AddShader("advanced-aa", "advanced-aa.vs", "advanced-aa.fs");
	AddShader("lottes", "lottes.vs", "lottes.fs");
	AddShader("pixellate", "def.vs", "pixellate.fs");
	AddShader("sharp-bilinear", "def.vs", "sharp-bilinear.fs");
	AddShader("AANN", "def.vs", "AANN.fs");
	//AddShader("fxaa", "def.vs", "fxaa.fs");
	// in case of pal-r57shell on intel internal graphic card, 
	// it seems it is related to the limitation of OpenGL compatibility of the intel graphic card
	// AddShader("pal-r57shell", "def.vs", "pal-r57shell.fs");  

	// Set Shader Mode
#if (0)
	m_ShaderMode = SM_CRT_SIMPLE;
#elif (0)
	m_ShaderMode = SM_CRT_LOTTES;
#elif (1)
	m_ShaderMode = SM_CRT_GEOM;
#elif (0)
	m_ShaderMode = SM_CRT_GEOM_HALATION;
#endif

	// Setup single or multi-phase(pass) shaders
	m_Phases["none"].push_back(GLPhase(GetShader("none"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEAREST));
	m_Phases["crt-simple"].push_back(GLPhase(GetShader("crt-simple"), glm::vec2(gVgaWidth, gVgaHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEAREST));
	m_Phases["lottes"].push_back(GLPhase(GetShader("lottes"),         glm::vec2(gVgaWidth, gVgaHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEAREST));
	m_Phases["crt-geom"].push_back(GLPhase(GetShader("crt-geom"), glm::vec2(gVgaWidth, gVgaHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEAREST));

	m_Phases["crt-geom-hal"].push_back(GLPhase(GetShader("crt-geom"), glm::vec2(gVgaWidth, gVgaHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_LINEAR));
	m_Phases["crt-geom-hal"].push_back(GLPhase(GetShader("gaussian-horiz"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_LINEAR));
	m_Phases["crt-geom-hal"].push_back(GLPhase(GetShader("gaussian-vert"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_LINEAR));
	m_Phases["crt-geom-hal"].push_back(GLPhase(GetShader("combine"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_LINEAR));
	/*
	switch (m_ShaderMode) {
		case SM_CRT_SIMPLE: {
			GLPhase ph0 = GLPhase(GetShader("crt-simple"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEAREST);
			phases.push_back(ph0);
			break;
		}
		case SM_CRT_LOTTES: {
			GLPhase ph0 = GLPhase(GetShader("lottes"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEAREST);
			phases.push_back(ph0);
			break;
		}
		case SM_CRT_GEOM: {
			GLPhase ph0 = GetShader("lottes"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEARESTGetShader("lottes"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEAREST;
			phases.push_back(ph0);
			break;
		}
		case SM_CRT_GEOM_HALATION: {
			GLPhase ph0 = GLPhase(GetShader("crt-geom"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_LINEAR);
			phases.push_back(ph0);
			GLPhase ph1 = GLPhase(GetShader("gaussian-horiz"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_LINEAR);
			phases.push_back(ph1);
			GLPhase ph2 = GLPhase(GetShader("gaussian-vert"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_LINEAR);
			phases.push_back(ph2);
			GLPhase ph3 = GLPhase(GetShader("combine"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_LINEAR);
			phases.push_back(ph3);
			break;
		}
		default: {
			GLPhase ph0 = GLPhase(GetShader("advanced-aa"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEAREST);
			phases.push_back(ph0);
			GLPhase ph1 = GLPhase(GetShader("crt-geom"), glm::vec2(m_WindowWidth, m_WindowHeight), glm::vec2(m_WindowWidth, m_WindowHeight), GL_NEAREST);
			phases.push_back(ph1);
			break;
		}
	}
	*/
	m_UsingShaderName = "crt-simple";
	GLPostProcess::Init();
	
	// Init Textures
	AddMenuTextureFromBackgroundImage("back");
	AddTextureFromFile("menu", "play1.bmp");
	AddTextureFromFile("play", "play1.bmp");

	// Set vertex and mesh
	std::vector<Vertex> vert = { 
		Vertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f), 
		Vertex(1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		Vertex(1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
		Vertex(0.0f, 1.0f, 0.0f, 0.0f, 1.0f), 
	};
	std::vector<GLuint> ind = { 0,1,2, 0,3,2 };	

	// Create mesh from textures
	AddMesh("back")->SetupMesh(vert, ind, GetTexture("back"));
	GetMesh("back")->SetPosition(glm::vec3(0.f, 0.f, 0.f));
	GetMesh("back")->SetSize(glm::vec3(m_WindowWidth, m_WindowHeight, 1.f));

	AddMesh("menu")->SetupMesh(vert, ind, GetTexture("menu"));
	GetMesh("menu")->SetPosition(glm::vec3(0.f, 0.f, 0.f));
	GetMesh("menu")->SetSize(glm::vec3(m_WindowWidth, m_WindowHeight, 1.f));

	AddMesh("play")->SetupMesh(vert, ind, GetTexture("play"));
	GetMesh("play")->SetPosition(glm::vec3(0.f, 0.f, 0.f));
	GetMesh("play")->SetSize(glm::vec3(m_WindowWidth, m_WindowHeight, 1.f));

	m_Framebuffer = std::make_shared<GLFramebuffer>(
		gVgaWidth, gVgaHeight, m_WindowWidth, m_WindowHeight, 8);
	m_Projection = glm::ortho(0.f, (float)m_WindowWidth, (float)m_WindowHeight, 0.f, -1.f ,1.f);
}

void SWOSGLRenderer::Finalize()
{
#ifdef IMGUI_OVERLAY
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
#endif
	SDL_GL_DeleteContext(m_Context);

	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

void SWOSGLRenderer::WindowResize(int w, int h)
{
	m_WindowWidth = w;
	m_WindowHeight = h;
}

void SWOSGLRenderer::SwitchWindowedFullscreen()
{
	m_FullScreenDesktop = !m_FullScreenDesktop;
	SDL_SetWindowFullscreen(m_Window, m_FullScreenDesktop ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	int w, h;
	SDL_GetWindowSize(m_Window, &w, &h);
	WindowResize(w, h);
}

Uint32 SWOSGLRenderer::setRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	return (a << 24) + (b << 16) + (g << 8) + r;
}

void SWOSGLRenderer::getRGBA(Uint32 color, Uint8* r, Uint8* g, Uint8* b, Uint8* a)
{
	*a = (color >> 24) & 255;
	*b = (color >> 16) & 255;
	*g = (color >> 8) & 255;
	*r = (color) & 255;
}

void SWOSGLRenderer::updateTestMenuPixels(Uint32* pixels)
{
	for (register int y = 0; y < gVgaHeight; y++) {
		for (register int x = 0; x < gVgaWidth; x++) {
			if (x >= 150 / 2 && x <= gVgaWidth - 150 / 2 && y >= 200 / 2 && y <= gVgaHeight - 200 / 2) {
				int randNo[3];
				for (int i = 0; i <= 2; i++) {
					randNo[i] = 1 + (rand() % 255);
				}
				pixels[y * gVgaWidth + x] = setRGBA(randNo[0], randNo[1], randNo[2], 128);
			}
			else {
				pixels[y * gVgaWidth + x] = setRGBA(0, 0, 0, 0);
			}
		}
	}
}

void SWOSGLRenderer::UpdateMenu()
{
	// Simulate menu items as a rectangle filled with random noise
	GLuint* vsPtr = new GLuint[gVgaWidth * gVgaHeight * 4];
	updateTestMenuPixels(vsPtr);
	GetTexture("menu")->Update(vsPtr);
	delete vsPtr;

#ifdef IMGUI_OVERLAY
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_Window);
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowBgAlpha(0.30f);
	ImGui::Begin("FPS Window", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Text("FPS: %.2f", io.Framerate);
	ImGui::Text("Window Size: %d x %d", m_WindowWidth, m_WindowHeight);
	ImGui::Text("Using Shader: %s", m_UsingShaderName.c_str());
	ImGui::DragFloat("Drag Opacity", &m_Opacity, 0.025f, 0.1f, 1.0f, "%.2f");
	if (ImGui::CollapsingHeader("Resolutions"))
	{
		static int modesel = -1;
		int display_mode_count = SDL_GetNumDisplayModes(gDisplayID);
		SDL_DisplayMode modename;
		for (int x = 0; x < display_mode_count; x++)
		{
			SDL_GetDisplayMode(gDisplayID, x, &modename);
			char modech[32];
			sprintf_s(modech, 31, "%d x %d - %d Hz", modename.w, modename.h, modename.refresh_rate);
			if (ImGui::Selectable(modech, x == modesel))
			{
				modesel = x;
				SDL_SetWindowDisplayMode(m_Window, &modename);
				SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN);
			}
		}
	}
	if (ImGui::CollapsingHeader("Shader"))
	{
		if (ImGui::Button("none"))
			m_UsingShaderName = "none";
		if (ImGui::Button("crt-simple"))
			m_UsingShaderName = "crt-simple";
		if (ImGui::Button("lottes"))
			m_UsingShaderName = "lottes";
		if (ImGui::Button("crt-geom"))
			m_UsingShaderName = "crt-geom";
		if (ImGui::Button("crt-geom-hal"))
			m_UsingShaderName = "crt-geom-hal";
	}
	ImGui::End();
#endif
}

void SWOSGLRenderer::RenderMenu()
{
	GLPostProcess::Begin(m_Phases.at(m_UsingShaderName));
#if (1)
	GetMesh("back")->Draw(GetShader("basic"), m_Projection);	// Draw "back" as Background
#else
	GetMesh("play")->Draw(GetShader("basic"), m_Projection);	// Draw "play" as Background
#endif
	GetMesh("menu")->SetColor(glm::vec4(1.f, 1.f, 1.f, m_Opacity));
	GetMesh("menu")->Draw(GetShader("basic"), m_Projection);	// Draw Menu items
	GLPostProcess::End(m_WindowWidth, m_WindowHeight);

	/*
	m_Framebuffer->Bind();
	glViewport(0, 0, gVgaWidth, gVgaHeight);
	GetMesh("back")->Draw(GetShader("basic"), m_Projection);	// Draw Background
	GetMesh("menu")->SetColor(glm::vec4(1.f, 1.f, 1.f, m_Opacity));
	GetMesh("menu")->Draw(GetShader("basic"), m_Projection);	// Draw Menu items
	m_Framebuffer->UnBind();

	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
	m_Framebuffer->Draw(m_WindowWidth, m_WindowHeight);
	*/
	
#ifdef IMGUI_OVERLAY
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

	SDL_GL_SwapWindow(m_Window);
}

void SWOSGLRenderer::UpdateGame()
{
	// Get and update Game pixels
#if (0)
	GLuint* vsPtr = new GLuint[gVgaWidth * gVgaHeight * 4];
	updateTestMenuPixels(vsPtr);
	GetTexture("play")->Update(vsPtr);
	delete vsPtr;
#endif

#ifdef IMGUI_OVERLAY
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_Window);
	ImGui::NewFrame();
	//
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowBgAlpha(0.30f);
	ImGui::Begin("FPS Window", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Text("FPS: %.2f", io.Framerate);
	ImGui::Text("Window Size: %d x %d", m_WindowWidth, m_WindowHeight);
	ImGui::Text("Using Shader: %s", m_UsingShaderName.c_str());
	if (ImGui::CollapsingHeader("Shader"))
	{
		if (ImGui::Button("none"))
			m_UsingShaderName = "none";
		if (ImGui::Button("crt-simple"))
			m_UsingShaderName = "crt-simple";
		if (ImGui::Button("lottes"))
			m_UsingShaderName = "lottes";
		if (ImGui::Button("crt-geom"))
			m_UsingShaderName = "crt-geom";
		if (ImGui::Button("crt-geom-hal"))
			m_UsingShaderName = "crt-geom-hal";
	}
	ImGui::End();
#endif
}

void SWOSGLRenderer::RenderGame()
{
	GLPostProcess::Begin(m_Phases.at(m_UsingShaderName));
	GetMesh("play")->Draw(GetShader("basic"), m_Projection);
	GLPostProcess::End(m_WindowWidth, m_WindowHeight);

	/*
	m_Framebuffer->Bind();
	glViewport(0, 0, gVgaWidth, gVgaHeight);
	GetMesh("play")->Draw(GetShader("basic"), m_Projection);
	m_Framebuffer->UnBind();
	
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
	m_Framebuffer->Draw(m_WindowWidth, m_WindowHeight);
	*/

#ifdef IMGUI_OVERLAY
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

	SDL_GL_SwapWindow(m_Window);
}

std::shared_ptr<GLShader> SWOSGLRenderer::AddShader(const std::string& name, const std::string& vs_fname, const std::string& fs_fname)
{
	const std::string& vs_fname_with_path = "shaders\\" + vs_fname;
	const std::string& fs_fname_with_path = "shaders\\" + fs_fname;

	if (!exists(vs_fname_with_path))
		std::cerr << "File not found:" << vs_fname << "\n";
	if (!exists(fs_fname_with_path))
		std::cerr << "File not found:" << fs_fname << "\n";
	std::string vertexCode;
	std::string fragmentCode;
	try
	{
		// Open files
		std::ifstream vertexShaderFile(vs_fname_with_path);
		std::ifstream fragmentShaderFile(fs_fname_with_path);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::exception e)
	{
		std::cerr << "SHADER: Failed to read shader files\n";
	}
	std::shared_ptr<GLShader> s = std::make_shared<GLShader>();
	s->LoadShaderFromCode(vertexCode, fragmentCode);
	m_Shaders[name] = s;
	std::cout << "SHADER " << name << " load from files: " << vs_fname << "," << fs_fname << "\n";
	return s;
}

std::shared_ptr<GLTexture> SWOSGLRenderer::AddTexture(const std::string& name)
{
	std::shared_ptr<GLTexture> txd = std::make_shared<GLTexture>();
	m_Textures[name] = txd;
	std::cout << "Create texture name " << name << "\n";
	return txd;
}

std::shared_ptr<GLTexture> SWOSGLRenderer::AddTextureFromFile(const std::string& name, const std::string& filename)
{
	if (!exists(filename))
	{
		std::cerr << "[ERROR]: Failed to load image filename: " << filename << "\n";
		return nullptr;
	}
	std::shared_ptr<GLTexture> txd = AddTexture(name);
	int width;
	int height;
	int chl;
	GLuint* data = (GLuint*)stbi_load(filename.c_str(), &width, &height, &chl, 0);
	if (chl == 3)
		txd->SetImageFormat(GL_RGB);
	else if (chl == 4)
		txd->SetImageFormat(GL_RGBA);

	txd->Generate(width, height, data);
	stbi_image_free(data);
	std::cout << "Loaded Image From File: " << filename << "\n";
	return txd;
}

std::shared_ptr<GLMesh> SWOSGLRenderer::AddMesh(const std::string& name)
{
	std::shared_ptr<GLMesh> mesh = std::make_shared<GLMesh>();
	m_Meshes[name] = mesh;
	return mesh;
}
