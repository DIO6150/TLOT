

/*
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <utils.hpp>

#include <iostream>

#define TITLE "The Legend of Teto"
#define SIZE_W 800
#define SIZE_H 800

#include <scene.hpp>
#include <mesh_manager.hpp>

int main (__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
	GLFWwindow* window;

	#ifdef __linux__
	glfwInitHint (GLFW_PLATFORM, GLFW_PLATFORM_X11);
	#endif

	if (!glfwInit ()) exit (-1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow (SIZE_W, SIZE_H, TITLE, NULL, NULL);
	if (!window) {
		glfwTerminate ();
		exit (-1);
	}
	glfwMakeContextCurrent (window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit (-1);
	}

	glViewport (0, 0, SIZE_W, SIZE_H);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	enableOpenGLDebugCallback ();

	// TEST DATA

	std::vector<Engine::Vertex> vertices = {
		{ -0.5f, -0.5f, 0.0f, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{  0.5f, -0.5f, 0.0f, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{  0.5f,  0.5f, 0.0f, 0.0, 0.0, 0.0, 0.0, 0.0 },
		{ -0.5f,  0.5f, 0.0f, 0.0, 0.0, 0.0, 0.0, 0.0 }
	};

	std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	{
		// INIT STUFF
		Engine::Shader shader1 {"<vs>assets/shaders/default.vertex", "<fs>assets/shaders/default.fragment"};
		Engine::Scene scene;

		Engine::MeshManager mesh_manager;
		Engine::Mesh *planeMesh = mesh_manager.CreateMesh (vertices, indices);

		Engine::GameObject *plane1 = scene.CreateObject (
		planeMesh, 
		{
			glm::mat4 {},
			{1.0, 1.0, 1.0},
			{0.0, 0.0, 0.0, 0.0},
		},
		&shader1);

		if (!plane1) {
			std::cout << "aahahahahaha\n";
			exit (-1);
		}

		// MAIN LOOP
		while (!glfwWindowShouldClose (window)) {
			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
			scene.Render ();

			glfwSwapBuffers (window);
			glfwPollEvents ();
		}
	}

	glfwDestroyWindow (window);
	glfwTerminate ();
	return (0);
}
*/

#include <component_manager.hpp>
#include <event_manager.hpp>
#include <mesh_manager.hpp>

#include <iostream>

struct DummyEvent {
	int magic;
};

int main (__attribute__((unused)) int argc, __attribute__((unused)) char ** argv) {
	Engine::ComponentManager 	compo_manager 	{};
	Engine::EventManager		event_manager 	{};
	Engine::MeshManager		mesh_manager	{};

	event_manager.RegisterEvent<DummyEvent> ();

	event_manager.Subscribe<DummyEvent> ([] (const DummyEvent & event) -> Engine::EventResult {
		std::cout << "Hello World : " << event.magic << "\n";
		return (Engine::EventResult::COMPLETE);
	}, Engine::ListenPriority::MEDIUM);

	event_manager.PostImmediate<DummyEvent> ({10});
}