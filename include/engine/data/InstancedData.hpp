#pragma once

#include <engine/core/Instancer.hpp>

#include <engine/core/AssetManager.hpp>
#include <engine/core/ResourceManager.hpp>

#include <engine/data/Material.hpp>
#include <engine/data/Mesh.hpp>
#include <engine/data/Model.hpp>

#include <string>
#include <map>

using namespace Engine::Core;
using namespace Engine::Data;


template<>
struct Engine::Core::Instance<Material>: InstanceBase {
public:
	Instance () = default;

	Instance (const Material & material) {
		diffuseTextures.assign (material.diffuseTextures.begin (), material.diffuseTextures.end ());
		diffuseColor = material.diffuseColor;
	}

	Instance (const HandleID & handle) :
		Instance {*AssetManager::GetInstance ()->GetMaterial (handle)}
		{

	}

	// TODO: have different layers for textures
	void SetTexture (const HandleID & texture) {
		diffuseTextures[0] = texture;
	}

	const std::vector<HandleID> & GetTextures () const {
		return (diffuseTextures);
	}

	const glm::vec3 & GetColor () const {
		return (diffuseColor);
	}

protected:
	std::vector<HandleID> diffuseTextures; // name, textureID

	glm::vec3 diffuseColor;
};

template<>
struct Engine::Core::Instance<Mesh>: InstanceBase {
	HandleID vertices;
	HandleID indices;

	Instance<Material> material;
	glm::mat4 transform;
	bool visible;

	std::string_view name;

	Instance () = default;

	Instance (const Mesh & mesh) :
		material {mesh.material}
		{
		vertices = mesh.vertices;
		indices  = mesh.indices;

		transform = mesh.transform;

		visible = true;
	}

	Instance (const HandleID & handle):
		Instance {*AssetManager::GetInstance ()->AssetManager::GetMesh (handle)}
		{
	}
};

template<>
struct Engine::Core::Instance<Model>: InstanceBase {
	std::vector<Instance<Mesh>> meshes;
	glm::mat4 transform;
	bool visible;

	Instance () = default;

	Instance (const Model & model) {
		meshes.reserve (model.meshes.size ());
		for (const auto & _meshID : model.meshes) {
			meshes.push_back (std::move (Instance<Mesh> {_meshID}));
		}

		transform = glm::mat4 {1.0};

		visible = true;
	}

	Instance (const HandleID & handle) :
		Instance {*AssetManager::GetInstance ()->AssetManager::GetModel (handle)}
		{
		
	}

	Instance<Mesh> & Find (const std::string & name) {
		for (auto & mesh : meshes) {
			if (mesh.name == name) {
				return mesh;
			}
		}
	}
};