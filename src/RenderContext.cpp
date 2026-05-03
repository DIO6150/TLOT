
#include <stdlib.h>

#include <stb/stb_image.h>

#include <core/Logger.hpp>

#include <opengl/RenderContext.hpp>
#include <opengl/Debug.hpp>

#include <core/InputManager.hpp>

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
		m_window = glfwCreateWindow (w, h, title.c_str (), glfwGetPrimaryMonitor (), NULL);
	}
	else
	{
		m_window = glfwCreateWindow (w, h, title.c_str (), NULL, NULL);
	}
	if (!m_window) {
		glfwTerminate ();
		exit (-1);
	}
	glfwMakeContextCurrent (m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Logger::panic ("Couldn't initialize GLAD.");
		exit (-1);
	}

	glfwSetInputMode (m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	m_width = w;
	m_height = h;

	InputManager::getInstance ().init (m_window);

}

void RenderContext::SetUpdateLoop (Callback updateCallback)
{
	m_update = updateCallback;
}

void RenderContext::SetRenderLoop (Callback renderCallback)
{
	m_render = renderCallback;
}

void RenderContext::Loop ()
{
	while (!glfwWindowShouldClose (m_window))
	{
		m_currentTime = glfwGetTime ();
		m_deltaTime = m_currentTime - m_lastTime;

		glfwGetCursorPos (m_window, &m_mouseX, &m_mouseY);

		Context context;
		context.mouseX      = m_mouseX;
		context.mouseY      = m_mouseY;
		context.lastMouseX  = m_lastMouseX;
		context.lastMouseY  = m_lastMouseY;
		context.currentTime = m_currentTime;
		context.deltaTime   = m_deltaTime;
		context.window      = m_window;

		m_update (context);

		m_lastMouseX = m_mouseX;
		m_lastMouseY = m_mouseY;

		
		if (m_currentTime - m_lastRenderTime < m_targetFrameTime) {
			InputManager::getInstance ().update (); // <-- poll events
			m_lastTime = m_currentTime;
			continue;
		}

		m_render (context);

		glfwSwapBuffers(m_window);
		InputManager::getInstance ().update (); // <-- poll events

		m_lastRenderTime = m_currentTime;
		m_lastTime       = m_currentTime;

	}
}

Viewport RenderContext::GetViewport ()
{
	return Viewport { m_width, m_height };
}

void RenderContext::HideMouse (bool flag)
{
	auto mode = flag ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
	glfwSetInputMode (m_window, GLFW_CURSOR, mode);
}

void RenderContext::Context::HideMouse (bool flag) const
{
	auto mode = flag ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
	glfwSetInputMode (window, GLFW_CURSOR, mode);
}
