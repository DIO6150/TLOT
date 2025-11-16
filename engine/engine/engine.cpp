#include <engine.hpp>

Engine::Engine::Engine () {

}

Engine::Handle Engine::Engine::loadShader (__attribute__ ((unused)) const std::string url, __attribute__ ((unused)) const ED::ShaderType type) {
	// TODO: fix shader import
	return (m_shader.create ());
}

Engine::Handle Engine::Engine::loadGeometry (__attribute__ ((unused)) const std::string url) {
	// TODO: fix geometry import
	return (m_geometry.create ());
}

Engine::Handle Engine::Engine::createMesh (Handle geometry_handle) {
	ED::Geometry & geometry = m_geometry.getRef (geometry_handle);

	Handle resource = m_mesh.create ();
	
	auto position = m_data_array.insert (m_data_array.begin () + resource.index, {});

	ED::Mesh * mesh = m_mesh.getRef (resource);
	mesh.geometry = &geometry;
	mesh.data = position.base ();

	return (resource);
}

void Engine::Engine::removeMesh (Handle mesh) {
	ED::Batch * location	= nullptr;
	ED::Mesh  * ptr		= m_mesh.get (mesh);

	if (!ptr) {
		++stat.failed_removal;
		return;
	}

	auto position = m_mesh_location.find (ptr);

	if (position != m_mesh_location.end ()) {
		location = position->second;

		location->removeMesh (*ptr);
	}

	m_mesh.destroy (mesh);
}