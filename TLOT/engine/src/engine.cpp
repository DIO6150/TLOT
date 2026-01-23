#include <engine.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


Engine::Engine::Engine () {
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

Engine::Engine::~Engine () {
	m_scenes.clear ();
	glfwDestroyWindow (window);
	glfwTerminate ();
}

Engine::Handle Engine::Engine::loadShader (__attribute__ ((unused)) const std::string url, __attribute__ ((unused)) const ED::ShaderType type) {
	// TODO: fix shader import
	return (m_shader.create ());
}

Engine::Handle Engine::Engine::loadGeometry (__attribute__ ((unused)) const std::string url) {
	// TODO: fix geometry import
	return (m_geometry.create ());
}

struct MeshBundle {
	std::vector<ED::Vertex> vertices;
	std::vector<ED::Vertex> indices;
};

MeshBundle processMesh(aiMesh *mesh, const aiScene *scene) {
    

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        
    }
    // process material
    if (mesh->mMaterialIndex >= 0) {

    }

    return MeshBundle ();
}

static void processNode(aiNode *node, const aiScene *scene, std::vector<MeshBundle> meshes) {
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));			
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
} 

void Engine::Engine::loadSceneAssimp (const std::string url) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(url, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
	{
		printf ("Assimp error: %s", importer.GetErrorString ());
		return;
	}
	//directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

Engine::Handle Engine::Engine::createMaterial () {
	// TODO: fix create material
	return (m_material.create ());
}

Engine::Scene * Engine::Engine::createScene () {
	m_scenes.emplace_back (&m_geometry);
	return (&m_scenes.back ());
}