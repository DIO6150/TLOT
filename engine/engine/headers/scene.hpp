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

	private:
		ResourceManager<ED::Geometry> *		m_geometry;
		
		ResourceManager<ED::Mesh> 		m_meshes;
		std::vector<ED::Batch>			m_batch_array;

		std::unordered_map<Handle, ED::Batch *> m_mesh_location;
	};
};