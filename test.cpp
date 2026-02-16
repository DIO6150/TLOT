
//#include "tests/rendering.hpp"

#include <filesystem>

#include <engine/modules/Camera.hpp>
#include <engine/modules/InstanceRenderer.hpp>
#include <engine/core/Scene.hpp>

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

using namespace Engine::Core;
using namespace Engine::Module;

int main (int argc, char ** argv) {
	#ifdef __linux__
	glfwInitHint (GLFW_PLATFORM, GLFW_PLATFORM_X11);
	#endif

	if (!glfwInit ()) exit (-1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow * window = glfwCreateWindow (800, 800, "TETO ENGINE", NULL, NULL);
	if (!window) {
		glfwTerminate ();
		exit (-1);
	}
	glfwMakeContextCurrent (window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit (-1);
	}

	glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace (GL_CCW);

	glViewport (0, 0, 800, 800);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glfwSwapInterval (0);

	Assimp::Importer importer;
	stbi_set_flip_vertically_on_load(0);

	Scene scene;

	for (int i = 1; i < argc; ++i) {
		std::cout << argv[i] << "\n";
		scene.Load (argv[i]);
	}

	InstanceRenderer renderer {&scene};
	Camera cam;
	renderer.Init (800, 800);

	renderer.AddPostProcessingEffect ("gray", scene.asset_manager.LoadShader ("gray", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/grayscale.postprocess.fragment"));
	renderer.AddPostProcessingEffect ("edge", scene.asset_manager.LoadShader ("edge", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/edge.postprocess.fragment"));

	//renderer.AddPostProcessingEffect ("bnw", scene.asset_manager.LoadShader ("bnw", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/black_and_white.postprocess.fragment"));
	//renderer.AddPostProcessingEffect ("inverse", scene.asset_manager.LoadShader ("inverse", "data/assets/shaders/blit_quad.vertex", "data/assets/shaders/inverse.postprocess.fragment"));
	
	double SENSITIVITY = 0.25f;
	double mouse_x, mouse_y;
	double last_x, last_y;
	//return 0;

	double lastRenderTime	= 0.0;
	double lastTime		= 0.0;
	double currentTime	= lastRenderTime;
	double deltaTime	= 0.0;

	double targetFPS = 144.0;
	double targetFrameTime = 1.0 / targetFPS;
	
	while (!glfwWindowShouldClose (window)) {
		currentTime = glfwGetTime ();
		deltaTime = currentTime - lastTime;

		glfwGetCursorPos (window, &mouse_x, &mouse_y);
		cam.rotate ({(mouse_x - last_x) * SENSITIVITY, (mouse_y - last_y) * -SENSITIVITY, 0.0});

		if (glfwGetKey (window, GLFW_KEY_W) == GLFW_PRESS) {
			cam.move_forward (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_S) == GLFW_PRESS) {
			cam.move_backward (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_A) == GLFW_PRESS) {
			cam.move_left (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_D) == GLFW_PRESS) {
			cam.move_right (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_E) == GLFW_PRESS) {
			cam.move_up (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_Q) == GLFW_PRESS) {
			cam.move_down (deltaTime);
		}

		if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose (window, true);
		}

		last_x = mouse_x;
		last_y = mouse_y;

		if (currentTime - lastRenderTime < targetFrameTime) {
			glfwPollEvents ();
			lastTime = currentTime;
			continue;
		}
		
		//printf ("Currently Rendering\n");
		renderer.Render (&cam);
		glfwSwapBuffers(window);
		glfwPollEvents ();

		lastRenderTime	= currentTime;
		lastTime	= currentTime;
	}

	exit (0);
}