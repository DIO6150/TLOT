#pragma once

#include <set>
#include <vector>
#include <memory>

#include <engine/batch.hpp>
#include <engine/mesh.hpp>
#include <engine/texture.hpp>
#include <engine/engine_types.hpp>

namespace Engine {
	class Engine;

	class Scene {
	public:
		Scene (Engine * engine); // dirty af
		~Scene ();

		Mesh *	createMesh (Geometry * geometry, Material * material);
		void	removeMesh (Mesh * mesh);

		Scene (const Scene &) = delete;
		Scene & operator= (const Scene &) = delete;

		Scene (Scene &&) = default;
		Scene & operator= (Scene &&) = default;

		std::vector<Mesh *> getMeshes ();

		void printStats ();

	private:
		Engine *						p_engine;

		std::unordered_map<Mesh *, std::unique_ptr<Mesh>>	m_meshes; // very cursed

		std::vector<Batch *>					m_batch_array;		// used for destruction purpose
		std::unordered_map<Shader *, Batch *>			m_shader_location;
		std::unordered_map<Mesh *, Batch *>			m_mesh_location;	// locate where a mesh instance is managed

		friend Engine;
		friend Mesh;
	};
};