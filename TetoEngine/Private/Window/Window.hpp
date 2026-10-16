#pragma once

#include <glfw/glfw3.h>

#include <TetoEngine/Common/Viewport.hpp>

#include <string>

namespace TetoEngine
{
	class Window
	{
	public:
		void init(uint32_t width, uint32_t h, std::string const & title, bool fullscreen = false);

		void resize(uint32_t width, uint32_t height);
		void setTitle(std::string name);

		Viewport getViewport() const;

		void swapBuffers() const;

		bool shouldClose() const;

	private:
		GLFWwindow * window_ = nullptr;
	};
};