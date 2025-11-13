#include <engine.hpp>

Engine::Handle Engine::Engine::createMesh (ED::Geometry & geometry) {
	Handle resource = m_mesh.create ();
	
	auto position = m_data_array.insert (m_data_array.begin () + resource.index, {});

	ED::Mesh & mesh = m_mesh.getRef (resource);
	mesh.geometry = &geometry;
	mesh.data = position.base ();

	return (resource);
}

void Engine::Engine::removeMesh (Handle mesh) {
	ED::Batch * location	= nullptr;
	ED::Mesh  * ptr		= m_mesh.get (mesh);

	if (!ptr) {
		return;
	}

	auto position = m_mesh_location.find (ptr);

	if (position != m_mesh_location.end ()) {
		location = position->second;

		location->removeMesh (*ptr);
	}

	bool result = m_mesh.destroy (mesh);
}