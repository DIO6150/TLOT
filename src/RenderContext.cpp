
#include <stdlib.h>

#include <stb/stb_image.h>

#include <core/Logger.hpp>

#include <RenderContext.hpp> // always first
#include <InputManager.hpp>

#include <Debugger/DebugMessages.hpp>



using namespace TLOT;

// TODO: Singleton ?, Delete opengl object when destroyed
// TODO: check if already init
void RenderContext::Init (size_t w, size_t h, std::string const & title, bool fullscreen)
{
	#ifdef __linux__
	// not all distros support wayland yet and I personnaly had problems when this flag was not set
	// maybe discard it or add it as an option
	glfwInitHint (GLFW_PLATFORM, GLFW_PLATFORM_X11);
	#endif

	if (!glfwInit ()) exit (-1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	if (fullscreen)
	{
		window = glfwCreateWindow (w, h, title.c_str (), glfwGetPrimaryMonitor (), NULL);
	}
	else
	{
		window = glfwCreateWindow (w, h, title.c_str (), NULL, NULL);
	}
	if (!window) {
		glfwTerminate ();
		exit (-1);
	}
	glfwMakeContextCurrent (window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Logger::panic ("Couldn't initialize GLAD.");
		exit (-1);
	}

	//glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// put this in renderer idk
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace (GL_CCW);

	glViewport (0, 0, (GLsizei)w, (GLsizei)h);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glfwSwapInterval (0);

	stbi_set_flip_vertically_on_load(1);

	enableOpenGLDebugCallback ();

	// Source - https://stackoverflow.com/a/68784846
	// Posted by anom1, modified by community. See post 'Timeline' for change history
	// Retrieved 2026-04-07, License - CC BY-SA 4.0

	std::cout << "" << std::endl;
	std::cout << "" << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "" << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "" << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "" << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "" << std::endl;

	std::string Vendor(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	std::string Renderer(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	std::string Version(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	std::string ShadingLanguageVersion(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

	std::string info = "";

	info += "\n";
	info += "OpenGL Vendor: " + Vendor + "\n";
	info += "OpenGL Renderer : " + Renderer + "\n";
	info += "OpenGL Version: " + Version + "\n";
	info += "OpenGL Shading Language Version: " + ShadingLanguageVersion + "\n";
	info += "\n";

	width = w;
	height = h;

	InputManager::getInstance ().init (window);

}

void RenderContext::Update ()
{
	currentTime = glfwGetTime ();
	deltaTime = currentTime - lastTime;

	glfwGetCursorPos (window, &mouseX, &mouseY);

	lastMouseX = mouseX;
	lastMouseY = mouseY;

	InputManager::getInstance ().update ();

	lastTime       = currentTime;
}

void RenderContext::Render ()
{
	lastRenderTime = currentTime;
	glfwSwapBuffers(window);
}

Viewport RenderContext::GetViewport () const
{
	return Viewport { width, height };
}

void RenderContext::HideMouse (bool flag)
{
	auto mode = flag ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
	glfwSetInputMode (window, GLFW_CURSOR, mode);
}

bool RenderContext::ShouldExit () const
{
	return glfwWindowShouldClose (window);
}

bool RenderContext::ShouldRenderFrame () const
{
	return currentTime - lastRenderTime >= targetFrameTime;
}
