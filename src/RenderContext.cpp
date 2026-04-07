
#include <stdlib.h>

#include <stb/stb_image.h>

#include <core/Logger.hpp>

#include <opengl/RenderContext.hpp>
#include <opengl/Debug.hpp>

using namespace TLOT;

// TODO: Singleton ?, Delete opengl object when destroyed

RenderContext::RenderContext (size_t w, size_t h, std::string const & title)
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

	m_window = glfwCreateWindow (w, h, title.c_str (), NULL, NULL);
	if (!m_window) {
		glfwTerminate ();
		exit (-1);
	}
	glfwMakeContextCurrent (m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Logger::panic ("Couldn't initialize GLAD.");
		exit (-1);
	}

	// TODO: port func from utils/Utils.hpp from the latest commits
	//enableOpenGLDebugCallback ();

	glfwSetInputMode (m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// put this in renderer idk
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace (GL_CCW);

	glViewport (0, 0, w, h);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glfwSwapInterval (0);

	stbi_set_flip_vertically_on_load(1);

	enableOpenGLDebugCallback ();
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
			glfwPollEvents ();
			m_lastTime = m_currentTime;
			continue;
		}

		m_render (context);

		glfwSwapBuffers(m_window);
		glfwPollEvents ();

		m_lastRenderTime = m_currentTime;
		m_lastTime       = m_currentTime;

	}
}
