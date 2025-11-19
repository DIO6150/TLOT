#include <scene.hpp>

Engine::Scene::Scene (ResourceManager<ED::Geometry> & geometry) :
	m_geometry {&geometry}
{
	
}

Engine::Handle Engine::Scene::createMesh (Handle geometry_handle, Handle material_handle) {

	auto & batch = m_batch_array.emplace_back ();

	ED::Geometry & geometry = m_geometry->getRef (geometry_handle);

	Handle resource = m_meshes.create ();
	
	auto position = m_data_array.insert (m_data_array.begin () + resource.index, {});

	ED::Mesh & mesh = m_meshes.getRef (resource);
	mesh.geometry = &geometry;
	mesh.position = {0.0, 0.0, 0.0};
	mesh.rotation = {0.0, 0.0, 0.0};
	mesh.scale    = {0.0, 0.0, 0.0};

	batch.addMesh (&mesh);

	return (resource);
}

void Engine::Scene::removeMesh (Handle mesh) {
	ED::Batch * location	= nullptr;
	ED::Mesh  * ptr		= m_meshes.get (mesh);

	if (!ptr) {
		//++stat.failed_removal;
		return;
	}

	auto position = m_mesh_location.find (mesh);

	if (position != m_mesh_location.end ()) {
		location = position->second;

		location->removeMesh (ptr);
	}

	m_meshes.destroy (mesh);
}