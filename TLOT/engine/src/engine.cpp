#include <engine.hpp>

#include <assimp/Importer.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


Engine::Engine::Engine () {
	GLFWwindow* window;

	#ifdef __linux__
	glfwInitHint (GLFW_PLATFORM, GLFW_PLATFORM_X11);
	#endif

	if (!glfwInit ()) exit (-1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow (800, 800, "TETO ENGINE", NULL, NULL);
	if (!window) {
		glfwTerminate ();
		exit (-1);
	}
	glfwMakeContextCurrent (window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit (-1);
	}

	glViewport (0, 0, 800, 800);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	Assimp::Importer importer;
}

Engine::Handle Engine::Engine::loadShader (__attribute__ ((unused)) const std::string url, __attribute__ ((unused)) const ED::ShaderType type) {
	// TODO: fix shader import
	return (m_shader.create ());
}

Engine::Handle Engine::Engine::loadGeometry (__attribute__ ((unused)) const std::string url) {
	// TODO: fix geometry import
	return (m_geometry.create ());
}

Engine::Handle Engine::Engine::createMaterial () {
	// TODO: fix create material
	return (m_material.create ());
}

Engine::Scene * Engine::Engine::createScene () {
	m_scenes.emplace_back (&m_geometry);
	return (&m_scenes.back ());
}