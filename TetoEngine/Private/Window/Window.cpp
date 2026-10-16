
#include <iostream>

#include <TetoEngine/Private/Window/Window.hpp>
#include <TetoEngine/Utils/Logger.hpp>

#include <stb/stb_image.h>

#include <glfw/glfw3.h>
#include <glad/glad.h>

using namespace TetoEngine;

void Window::init(uint32_t width, uint32_t height, std::string const & title, bool fullscreen = false)
{
	#ifdef __linux__
	// not all distros support wayland yet and I personnaly had problems when this flag was not set
	// maybe discard it or add it as an option
	glfwInitHint (GLFW_PLATFORM, GLFW_PLATFORM_X11);
	#endif

	if (!glfwInit()) exit (-1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	if (fullscreen)
	{
		window_ = glfwCreateWindow(width, height, title.c_str(), glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		window_ = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	}

	if (!window_) {
		glfwTerminate ();
		exit (-1);
	}

	glfwMakeContextCurrent(window_);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Logger::panic("Couldn't initialize GLAD.");
		exit(-1);
	}

	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glfwSwapInterval(0);

	stbi_set_flip_vertically_on_load(1);

	//enableOpenGLDebugCallback();

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
}

void Window::resize(uint32_t width, uint32_t height)
{
	glfwSetWindowSize(window_, width, height);
}

void Window::setTitle(std::string name)
{
	glfwSetWindowTitle(window_, name.c_str());
}

void Window::swapBuffers() const
{
	glfwSwapBuffers(window_);
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(window_);
}