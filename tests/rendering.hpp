#include <iostream>
#include <ctime>

#include <engine/engine.hpp>
#include <engine/utils.hpp>

#include <glm/glm.hpp>

using namespace Engine;

namespace TestRendering {	
	static bool test () {

		double SENSITIVITY = 0.25f;

		double mouse_x, mouse_y;
    		double last_x, last_y;

		Engine::Engine engine;

		enableOpenGLDebugCallback ();
		//Engine::Scene * scene = engine.loadSceneAssimp ("data/assets/models/columbina_rigged_free/scene.gltf");
		//Engine::Scene * scene = engine.loadSceneAssimp ("data/assets/models/genshin_impact_-_chiori/scene.gltf");
		Engine::Scene * scene = engine.loadSceneAssimp ("data/assets/models/eula_genshin_impact/scene.gltf");

		scene->printStats ();

		auto meshes = scene->getMeshes ();

		Engine::Camera & cam = engine.getCamera ();

		glm::vec3 dir[] = {
			{1.0, 0.0, 0.0},
			{0.0, 1.0, 1.0},
			{-1.0, 0.0, 1.0},
			{0.0, -1.0, 0.5}
		};

		int cdir;

		while (!glfwWindowShouldClose (engine.getWindow ())) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cdir = 0;
			for (const auto & m : meshes) {
				m->translate (dir[++cdir % (sizeof (dir) / sizeof (glm::vec3))] * 0.0005f);
			}

			engine.render (scene);
			
			glfwSwapBuffers(engine.getWindow ());

			glfwPollEvents ();

			glfwGetCursorPos (engine.getWindow (), &mouse_x, &mouse_y);

			cam.rotate ({(mouse_x - last_x) * SENSITIVITY, (mouse_y - last_y) * -SENSITIVITY, 0.0});

			if (glfwGetKey (engine.getWindow (), GLFW_KEY_W) == GLFW_PRESS) {
				cam.move_forward ();
			}

			if (glfwGetKey (engine.getWindow (), GLFW_KEY_S) == GLFW_PRESS) {
				cam.move_backward ();
			}

			if (glfwGetKey (engine.getWindow (), GLFW_KEY_A) == GLFW_PRESS) {
				cam.move_left ();
			}

			if (glfwGetKey (engine.getWindow (), GLFW_KEY_D) == GLFW_PRESS) {
				cam.move_right ();
			}

			if (glfwGetKey (engine.getWindow (), GLFW_KEY_SPACE) == GLFW_PRESS) {
				cam.move_up ();
			}

			if (glfwGetKey (engine.getWindow (), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				cam.move_down ();
			}

			if (glfwGetKey (engine.getWindow (), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose (engine.getWindow (), true);
			}

			last_x = mouse_x;
			last_y = mouse_y;
		}

		return (true);
	}
}