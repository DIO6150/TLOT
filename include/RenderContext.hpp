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

	struct RenderContext
	{
		void Init (size_t w, size_t h, std::string const & title, bool fullscreen = false);

		void Update ();
		void Render ();
		bool ShouldExit () const;
		bool ShouldRenderFrame () const;

		void HideMouse (bool flag);

		Viewport GetViewport () const;

		GLFWwindow * window = nullptr;

		int width  = 0;
		int height = 0;

		double mouseX     = 0.0;
		double mouseY     = 0.0;
		double lastMouseX = 0.0;
		double lastMouseY = 0.0;

		double lastRenderTime  = 0.0;
		double lastTime        = 0.0;
		double currentTime     = 0.0;
		double deltaTime       = 0.0;
		double targetFPS       = 144.0;
		double targetFrameTime = 1.0 / targetFPS;

		bool shoudExit = false;

		RenderContext () = default;
	};
}