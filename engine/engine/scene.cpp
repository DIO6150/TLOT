#include <scene.hpp>

Engine::Scene::Scene (ResourceManager<ED::Geometry> & geometry) :
	m_geometry {&geometry}
{
	
}

Engine::Handle Engine::Scene::createMesh (Handle geometry_handle, Handle material_handle) {
	ED::Batch 	* 	batch;

	auto pos = m_batch_array.find (material_handle);

	if (pos == m_batch_array.end ()) {
		batch = new ED::Batch ();
		m_batch_array.emplace (material_handle, batch);
	}
	else {
		batch = pos->second;
	}

	ED::Geometry & geometry = m_geometry->getRef (geometry_handle);

	Handle resource = m_meshes.create (
		this,
		&geometry,
		glm::vec3 (0.0),
		glm::vec3 (0.0),
		glm::vec3 (0.0)
	);

	ED::Mesh * mesh = m_meshes.get (resource);

	if (!mesh) {
		exit (EXIT_FAILURE);
	}

	batch->addMesh (mesh);

	m_mesh_location.emplace (resource, batch);

	return (resource);
}

void Engine::Scene::removeMesh (Handle mesh) {
	ED::Batch * location	= nullptr;
	ED::Mesh  * ptr		= m_meshes.get (mesh);

	if (!ptr) {
		++m_failed_removal;
		return;
	}

	auto position = m_mesh_location.find (mesh);

	if (position != m_mesh_location.end ()) {
		location = position->second;

		location->removeMesh (ptr);
	}

	m_meshes.destroy (mesh);
}

void Engine::Scene::printStats () {
	printf ("[Batch Info]\n");
	printf ("There is %llu batch(es)\n", m_batch_array.size ());

	size_t index = 0;
	for (const auto & [material, batch] : m_batch_array) {
		size_t gis  = batch->m_geometry_indices.size ();
		size_t ges  = batch->m_geometry_entries.size () * sizeof (ED::Batch::GeometryEntry);
		size_t cmds = batch->m_commands.size () * sizeof (ED::DrawCommand);

		size_t mis  = batch->m_mesh_indices.size ();
		size_t mes  = batch->m_mesh_entries.size () * sizeof (ED::Batch::MeshEntry);
		size_t inss = batch->m_instances.size () * sizeof (ED::InstanceData); 
		printf ("\t[%llu]\t{GIS: %llu elements; GES: %llu bytes; CMDS: %llu bytes}\n\t\t{MIS: %llu elements; MES: %llu bytes; INSS: %llu bytes}\n", 
				index,
				gis, ges, cmds,
				mis, mes, inss
			);

		++index;
	}

	printf ("[Miscellaneous]\n");
	printf ("Failed to remove %d mesh(es).\n", m_failed_removal);
}