#include <scene.hpp>

Engine::Scene::Scene (ResourceManager<ED::Geometry> * geometry) :
	m_geometry {geometry},
	m_failed_removal {0} {
	
}

Engine::Scene::~Scene () {
	printf ("destructor called.\n");
	for (auto batch : m_batch_array) {
		printf ("%p, %d\n", batch, batch->m_mesh_count);
		delete batch;
	}
}

Engine::Handle Engine::Scene::createMesh (Handle geometry_handle, Handle material_handle) {
	ED::Batch 	* 	batch;

	auto pos = m_material_location.find (material_handle);

	if (pos == m_material_location.end ()) {
		batch = new ED::Batch {};
		printf ("batch created with new : %p\n", batch);
		m_material_location.emplace (material_handle, batch);
		m_batch_array.push_back (batch);
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
		// TODO: print log + handle error
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
	printf ("--------------------------------------------------------------------------------------------------\n");
	printf ("[Batch Info]\n");
	printf ("There is %lu batch(es)\n", m_material_location.size ());

	size_t index = 0;
	for (const auto & [material, batch] : m_material_location) {
		size_t gis  = batch->m_geometry_indices.size ();
		size_t ges  = batch->m_geometry_entries.size () * sizeof (ED::Batch::GeometryEntry);
		size_t cmds = batch->m_commands.size () * sizeof (ED::DrawCommand);

		size_t mis  = batch->m_mesh_indices.size ();
		size_t mes  = batch->m_mesh_entries.size () * sizeof (ED::Batch::MeshEntry);
		size_t inss = batch->m_instances.size () * sizeof (ED::InstanceData); 
		printf ("\t[%lu]\t{GIS: %lu elements; GES: %lu bytes; CMDS: %lu bytes}\n\t\t{MIS: %lu elements; MES: %lu bytes; INSS: %lu bytes}\n", 
				index,
				gis, ges, cmds,
				mis, mes, inss
			);

		++index;
	}

	printf ("[Miscellaneous]\n");
	printf ("Failed to remove %d mesh(es).\n", m_failed_removal);

	printf ("--------------------------------------------------------------------------------------------------\n");
}