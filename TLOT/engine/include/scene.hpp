#pragma once

#include <vector>

#include <batch.hpp>
#include <mesh.hpp>
#include <resource_manager.hpp>

namespace Engine {
	class Scene {
	public:
		Scene (ResourceManager<ED::Geometry> & geometry);

		Handle 	createMesh (Handle geometry, Handle material_handle);
		void 	removeMesh (Handle mesh);

		Scene (const Scene &) = delete;
		Scene & operator= (const Scene &) = delete;

		Scene (Scene &&) = default;
		Scene & operator= (Scene &&) = default;

		void printStats ();

	private:
		ResourceManager<ED::Geometry> *		m_geometry;
		
		ResourceManager<ED::Mesh> 		m_meshes;
		std::unordered_map<Handle, ED::Batch *>	m_batch_array; // material_handle -> batch

		std::unordered_map<Handle, ED::Batch *> m_mesh_location; // locate where a mesh instance is managed

		uint32_t	m_failed_removal;
	};
};