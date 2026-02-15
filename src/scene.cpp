#include <engine/core/Scene.hpp>

#include <engine/data/Instance.hpp>
#include <engine/data/Geometry.hpp>
#include <engine/data/Shader.hpp>
#include <engine/data/Texture.hpp>
#include <engine/data/Vertex.hpp>

#include <filesystem>

using namespace Engine::Core;
using namespace Engine::Data;


static Vector<String> getTexturePaths (aiMaterial * material, aiTextureType type) {
	Vector<String> paths;
	for(unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
		aiString str;
		material->GetTexture(type, i, &str);
		paths.push_back (std::string {str.C_Str ()});
	}
	
	return (paths);
}



Scene::Scene ()
	{
	defaultShader	= asset_manager.LoadShader	("default", "data/assets/shaders/default.vertex", "data/assets/shaders/default.fragment");
	defaultTexture	= asset_manager.LoadTexture	("default", "data/assets/textures/default.png");
	//defaultMaterial	= asset_manager.createMaterial	("default", defaultShader, {defaultTexture});
}

void Scene::Load (const std::string & url) {
	std::string directory = url.substr(0, url.find_last_of('/'));

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(url, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::string error {importer.GetErrorString ()};
		throw std::runtime_error ("Assimp error " + error);
		return;
	}
	
	ProcessNode(scene->mRootNode, scene, directory, graph.GetRoot ());
}

void Scene::ProcessMesh (aiMesh *mesh, __attribute__((unused)) const aiScene * scene, const aiMatrix4x4 & transform, const std::string & directory, SceneNode * node) {
	String			mesh_name;
	String			material_name;

	Vector<Vertex>		vertices;
	Vector<uint32_t>	indices;

	GeometryID		geometry;
	MaterialID		material;
	Vector<TextureID>	diffuseIDs;

	mesh_name = {mesh->mName.C_Str ()};

	if (auto opt = asset_manager.GetGeometryID (mesh_name)) {
		geometry = opt.value ();
	}
	else {
		// Vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			float tex_coord_x = 0.0f;
			float tex_coord_y = 0.0f;

			if (mesh->HasTextureCoords (0)) { // TODO-fix: maybe try to have all tex coords, idk how
				tex_coord_x = mesh->mTextureCoords[0][i].x;
				tex_coord_y = mesh->mTextureCoords[0][i].y;
			}

			vertices.emplace_back (
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
				indices.push_back (face.mIndices[j]);
			}
		}

		geometry = asset_manager.CreateGeometry (mesh_name, vertices, indices);
	}

	// TODO-fix : Implement flat color materials
	aiMaterial * ai_mat = scene->mMaterials[mesh->mMaterialIndex];
	material_name = ai_mat->GetName ().C_Str ();
	
	if (auto opt = asset_manager.GetMaterialID (material_name)) {
		material = opt.value ();

		Material _material = asset_manager.GetMaterial (material);
		diffuseIDs = _material.diffuse_textures;
	}
	else {
		for (auto path: getTexturePaths (ai_mat, aiTextureType_DIFFUSE)) {
			std::string tex_name = std::filesystem::path {path}.stem ().string ();
			ObjectID diffuse = asset_manager.LoadTexture (tex_name, directory + "/" + path);

			if (!diffuse.isValid ()){
				diffuse = defaultTexture;
			}

			diffuseIDs.push_back (diffuse);
		}

		//if (diffuseIDs.size () < 1) {
		//	diffuseIDs.push_back (defaultTexture);
		//}

		material = asset_manager.CreateMaterial (material_name, defaultShader, diffuseIDs); // TODO: modify shader I guess ?
	}

	// TODO-fix: generate mesh pos from aiMatrix4x4 or similar
	node->addMesh (geometry, material);
}

void Scene::ProcessNode(aiNode * node, const aiScene * scene, const std::string & directory, SceneNode * graph_node) {
	//if (graph.GetTotalMeshCount () > 1) return;

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh (mesh, scene, node->mTransformation, directory, graph_node);
		//break;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		SceneNode * nnode = new SceneNode {node->mName.C_Str (), graph_node, graph_node->graph};
		ProcessNode (node->mChildren[i], scene, directory, nnode);
	}
}