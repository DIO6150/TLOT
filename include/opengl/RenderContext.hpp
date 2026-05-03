#pragma once

#include <string>

#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace TLOT
{
	struct Viewport
	{
		int width, height;
		float AspectRatio() const { return static_cast<float>(width) / height; }
	};

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

			void HideMouse (bool flag) const;
		};

		using Callback = std::function<void (Context const &)>;

		void Init (size_t w, size_t h, std::string const & title, bool fullscreen = false);

		void SetUpdateLoop (Callback updateCallback);
		void SetRenderLoop (Callback renderCallback);

		void HideMouse (bool flag);

		void Loop ();

		Viewport GetViewport ();

		RenderContext () = default;
	
	private:
		GLFWwindow * m_window = nullptr;

		Callback m_update = [] (Context const &) {};
		Callback m_render = [] (Context const &) {};

		int m_width  = 0;
		int m_height = 0;

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