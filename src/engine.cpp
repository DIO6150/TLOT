
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/engine.hpp>
#include <engine/utils.hpp>

static std::vector<std::string> getTexturePaths (aiMaterial * material, aiTextureType type) {
	std::vector<std::string> paths;
	for(unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
		aiString str;
		material->GetTexture(type, i, &str);
		paths.push_back (std::string {str.C_Str ()});
	}
	return (paths);
}

struct MeshBundle {
	std::string name;
	std::vector<Engine::Vertex> vertices;
	std::vector<uint32_t> indices;

	std::string 		 material_name;
	std::vector<std::string> diffuse_textures;
	std::vector<std::string> specular_textures;
};

static MeshBundle processMesh(aiMesh *mesh, __attribute__((unused)) const aiScene * scene) {

	MeshBundle bmesh;
	bmesh.vertices	.reserve(mesh->mVertices->Length ());
	bmesh.indices	.reserve(mesh->mFaces->mNumIndices);

	bmesh.name = mesh->mName.C_Str ();

	/*
		GEOMETRY
	*/

	// Vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		float tex_coord_x = 0.0f;
		float tex_coord_y = 0.0f;

		if (mesh->HasTextureCoords (0)) { // TODO-fix: maybe try to have all tex coords, idk how
			tex_coord_x = mesh->mTextureCoords[0][i].x;
			tex_coord_y = mesh->mTextureCoords[0][i].y;
		}

		bmesh.vertices.emplace_back (
			mesh->mVertices[i].x,		mesh->mVertices[i].y,		mesh->mVertices[i].z,
			mesh->mNormals[i].x,		mesh->mNormals[i].y,		mesh->mNormals[i].z,
			mesh->mTangents[i].x,		mesh->mTangents[i].y,		mesh->mTangents[i].z,
			tex_coord_x,			tex_coord_y
		);
	}

	// Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];

		for(unsigned int j = 0; j < face.mNumIndices; ++j) {	
			bmesh.indices.push_back (face.mIndices[j]);
		}
	}

	/*
		MATERIAL
	*/

	aiMaterial * material	= scene->mMaterials[mesh->mMaterialIndex];
	bmesh.material_name	= material->GetName ().C_Str ();
	bmesh.diffuse_textures	= getTexturePaths (material, aiTextureType_DIFFUSE);
	bmesh.specular_textures = getTexturePaths (material, aiTextureType_SPECULAR);

	return (bmesh);
}

static void processNode(aiNode *node, const aiScene *scene, std::vector<MeshBundle> & meshes) {
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back (processMesh (mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode (node->mChildren[i], scene, meshes);
	}
} 

Engine::Engine::Engine () {
	#ifdef __linux__
	glfwInitHint (GLFW_PLATFORM, GLFW_PLATFORM_X11);
	#endif

	if (!glfwInit ()) exit (-1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	m_window = glfwCreateWindow (800, 800, "TETO ENGINE", NULL, NULL);
	if (!m_window) {
		glfwTerminate ();
		exit (-1);
	}
	glfwMakeContextCurrent (m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit (-1);
	}

	glfwSetInputMode (m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace (GL_CCW);

	glViewport (0, 0, 800, 800);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	Assimp::Importer importer;
	stbi_set_flip_vertically_on_load(0);

	m_atlas.build (0);
}

Engine::Engine::~Engine () {
	m_scenes.clear ();
	glfwDestroyWindow (m_window);
	glfwTerminate ();
}

Engine::Camera & Engine::Engine::getCamera () {
	return (m_camera);
}

Engine::Shader * Engine::Engine::loadShader (const std::string & name, const std::string & vertex_url, const std::string & frag_url) {
	auto pos = m_shader.try_emplace (name, std::make_unique<Shader> (vertex_url, frag_url));
	return (pos.first->second.get ());
}

Engine::Geometry * Engine::Engine::loadGeometry (__attribute__((unused)) const std::string & name, __attribute__((unused)) const std::string url) {
	// TODO: fix geometry import
	return (nullptr);
}

Engine::Material * Engine::Engine::createMaterial (const std::string & name, Shader * shader, Texture * diffuse) {
	auto pos = m_material.try_emplace (name, std::make_unique<Material> (shader, m_atlas.getQuad (diffuse)));
	return (pos.first->second.get ());
}

Engine::Geometry * Engine::Engine::createGeometry (const std::string & name, std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
	auto pos = m_geometry.try_emplace (name, std::make_unique<Geometry> (vertices, indices));
	return (pos.first->second.get ());
}

Engine::Texture * Engine::Engine::loadTexture (const std::string & name, const std::string url, TexType type) {
	auto pos = m_texture.try_emplace (name, std::make_unique<Texture> (name, url, type));
	return (pos.first->second.get ());
}

Engine::Scene * Engine::Engine::createScene () {
	m_scenes.emplace_back (this);
	return (&m_scenes.back ());
}

Engine::Scene * Engine::Engine::loadSceneAssimp (const std::string url) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(url, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf ("Assimp error: %s", importer.GetErrorString ());
		return (nullptr);
	}
	
	std::string directory = url.substr(0, url.find_last_of('/'));

	Scene * _scene = createScene ();

	std::vector<MeshBundle> meshes;
	processNode(scene->mRootNode, scene, meshes);

	// TODO-fix: very naive way of importing meshs in a scene
	for (const auto & mesh : meshes) {
		//printf ("(DEBUG): Diffuse Count = %llu\n", mesh.diffuse_textures.size ());
		//printf ("(DEBUG): Specular Count = %llu\n", mesh.specular_textures.size ());
		//if (mesh.name == "Object_2") continue;

		Texture * diffuse = nullptr;
		std::string material_name = "default";

		if (mesh.diffuse_textures.size () > 0) {
			std::string file_name = std::filesystem::path {mesh.diffuse_textures[0]}.stem ().string ();
			diffuse = loadTexture (file_name, directory + "/" + mesh.diffuse_textures[0], TexType::DIFFUSE);

			material_name = mesh.material_name;
		}
		
		if (mesh.specular_textures.size () > 0) {
			loadTexture (mesh.specular_textures[0], directory + "/" + mesh.specular_textures[0], TexType::SPECULAR);
		}
		
		if (diffuse == nullptr) {
			//printf ("Careful, texture is null for mesh %s.\n", mesh.name.c_str ());
			// TODO-fix: add flat color material
		}
		Material * material = createMaterial (
			material_name,
			loadShader ("default", "data/assets/shaders/default.vertex", "data/assets/shaders/default.fragment"),
			diffuse
		);
		Geometry * geometry = createGeometry (mesh.name, mesh.vertices, mesh.indices);
		_scene->createMesh (geometry, material);
	}


	// TODO-fix: I want to kill myself

	return (_scene);
}

void Engine::Engine::render (Scene * scene) {
	glm::mat4 view;
	view = m_camera.getView ();

	glm::mat4 projection;

	projection = glm::perspective(glm::radians (45.0f), 1.0f, 0.1f, 100.0f);

	for (const auto & batch : scene->m_batch_array) {
		batch->syncInstances	();
		batch->syncCommands	();
		batch->bind		();
		batch->getShader	()->use ();

		// TODO: put the names of the uniform in the settings like map = {"projection": "uProjection"}, {"view": "uView"}
		batch->getShader ()-> uploadMatrix4 ("uProjection", projection);
		batch->getShader ()-> uploadMatrix4 ("uView", view);

		// TODO: manage multiple atlases, one for each texture type maybe ?
		batch->getShader ()-> upload1i ("uAtlas", 0);
		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D_ARRAY, m_atlas.texture_handle);

		//printf ("DEBUG: Mesh count = %d\n", batch->getMeshCount ());

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, batch->getDrawCommandCount (), 0);
	
		batch->getShader ()->cancel ();
		batch->unbind ();
	}
}

GLFWwindow * Engine::Engine::getWindow () {
	return (m_window);
}