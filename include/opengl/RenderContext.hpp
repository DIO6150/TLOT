#pragma once

#include <string>

#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace TLOT
{
	class RenderContext
	{
	
	public:
		struct Context
		{
			double mouseX;
			double mouseY;

			double lastMouseX;
			double lastMouseY;

			double currentTime;
			double deltaTime;

			GLFWwindow * window;
		};

		using Callback = std::function<void (Context const &)>;

		RenderContext (size_t w, size_t h, std::string const & title);

		void SetUpdateLoop (Callback updateCallback);
		void SetRenderLoop (Callback renderCallback);

		void Loop ();
	
	private:
		GLFWwindow * m_window;

		Callback m_update = [] (Context const &) {};
		Callback m_render = [] (Context const &) {};

		double m_mouseX     = 0.0;
		double m_mouseY     = 0.0;
		double m_lastMouseX = 0.0;
		double m_lastMouseY = 0.0;

		double m_lastRenderTime  = 0.0;
		double m_lastTime        = 0.0;
		double m_currentTime     = 0.0;
		double m_deltaTime       = 0.0;
		double m_targetFPS       = 144.0;
		double m_targetFrameTime = 1.0 / m_targetFPS;
	};
}