#pragma once

#include <engine/core/Instancer.hpp>

#include <engine/core/AssetManager.hpp>
#include <engine/core/ResourceManager.hpp>
#include <engine/utils/Utils.hpp>

#include <engine/data/Material.hpp>
#include <engine/data/Mesh.hpp>
#include <engine/data/Model.hpp>

#include <iostream>
#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scaling;
	bool visible;

	std::string_view name;

	Instance () = default;

	Instance (const Mesh & mesh) :
		material {mesh.material}
		{
		vertices = mesh.vertices;
		indices  = mesh.indices;

		position = mesh.position;
		rotation = mesh.rotation;
		scaling  = mesh.scaling;

		visible = true;
	}

	Instance (const HandleID & handle):
		Instance {*AssetManager::GetInstance ()->AssetManager::GetMesh (handle)}
		{
	}

	void Translate (glm::vec3 position) {
		this->position = position;
	}

	void Rotate (glm::vec3 rotation) {
		this->rotation = rotation;
	}

	void Scale (glm::vec3 scalar) {
		this->scaling = scalar;
	}

	glm::mat4 GetMatrix () const {
		glm::mat4 transform = glm::mat4 (1.0);
		transform = glm::translate (transform, position);
		transform = glm::rotate (transform, glm::radians (rotation.z), glm::vec3 {0.0, 0.0, 1.0});
		transform = glm::rotate (transform, glm::radians (rotation.y), glm::vec3 {0.0, 1.0, 0.0});
		transform = glm::rotate (transform, glm::radians (rotation.z), glm::vec3 {1.0, 0.0, 0.0});
		transform = glm::scale  (transform, scaling);
		return (transform);
	}
};

template<>
struct Engine::Core::Instance<Model>: InstanceBase {
	std::vector<Instance<Mesh>> meshes;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scaling;
	bool visible;

	Instance () = default;

	Instance (const Model & model) {
		meshes.reserve (model.meshes.size ());
		for (const auto & _meshID : model.meshes) {
			meshes.push_back (std::move (InstanceFactory<Mesh>::CreateInstance (_meshID)));
		}

		position = glm::vec3 {0.0};
		rotation = glm::vec3 {0.0};
		scaling  = glm::vec3 {1.0};

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

	void Translate (glm::vec3 position) {
		this->position += position;
	}

	void Rotate (glm::vec3 rotation) {
		this->rotation += rotation;

		if (this->rotation.x > 360.0) this->rotation.x = 0.0;
		if (this->rotation.y > 360.0) this->rotation.y = 0.0;
		if (this->rotation.z > 360.0) this->rotation.z = 0.0;

		if (this->rotation.x < 0.0) this->rotation.x = 360.0;
		if (this->rotation.y < 0.0) this->rotation.y = 360.0;
		if (this->rotation.z < 0.0) this->rotation.z = 360.0;
	}

	void Scale (glm::vec3 scalar) {
		this->scaling += scalar;
	}

	glm::mat4 GetMatrix () const {
		glm::mat4 transform = glm::mat4 (1.0);
		transform = glm::translate (transform, position);
		transform = glm::rotate (transform, glm::radians (rotation.z), glm::vec3 {0.0, 0.0, 1.0});
		transform = glm::rotate (transform, glm::radians (rotation.y), glm::vec3 {0.0, 1.0, 0.0});
		transform = glm::rotate (transform, glm::radians (rotation.x), glm::vec3 {1.0, 0.0, 0.0});
		transform = glm::scale  (transform, scaling);
		return (transform);
	}
};