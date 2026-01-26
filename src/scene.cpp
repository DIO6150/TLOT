#include <engine/scene.hpp>
#include <engine/engine.hpp>

Engine::Scene::Scene (Engine * engine) :
	p_engine {engine} {
	
}

Engine::Scene::~Scene () {
	for (auto batch : m_batch_array) {
		delete batch;
	}
}

Engine::Mesh * Engine::Scene::createMesh (Geometry * geometry, Material * material) {
	Batch * batch;
	
	auto shader_pos = m_shader_location.find (material->shader);
	if (shader_pos == m_shader_location.end ()) {
		batch = new Batch;
		batch->attachShader (material->shader);
		
		m_shader_location.emplace (material->shader, batch);
		m_batch_array.push_back (batch);
	}
	else {
		batch = shader_pos->second;
	}
	
	auto ptr = std::make_unique<Mesh> (
		this,
		geometry,
		glm::vec3 (0.0),
		glm::vec3 (0.0),
		glm::vec3 (1.0),
		material
	);
	
	auto mesh_pos = m_meshes.emplace (ptr.get (), std::move (ptr));
	
	if (!mesh_pos.second) {
		printf ("ERROR: couldnt emplace the mesh");
		// TODO: print log + handle error
		exit (EXIT_FAILURE);
	}
	

	Mesh * mesh = mesh_pos.first->first;
	batch->addMesh (mesh);
	m_mesh_location.emplace (mesh, batch);
	return (mesh);
}

void Engine::Scene::removeMesh (Mesh * mesh) {
	Batch * location = nullptr;

	auto position = m_mesh_location.find (mesh);

	if (position != m_mesh_location.end ()) {
		location = position->second;
		location->removeMesh (mesh);
	}

	m_meshes.erase (m_meshes.find (mesh));
}

std::vector<Engine::Mesh *> Engine::Scene::getMeshes () {
	std::vector<Mesh *> result;

	for (const auto & [mesh_ptr, mesh_uptr] : m_meshes) {
		result.push_back (mesh_ptr);
	}

	return (result);
}

void Engine::Scene::printStats () {
	printf ("--------------------------------------------------------------------------------------------------\n");
	printf ("[Batch Info]\n");
	printf ("There is %llu batch(es)\n", m_batch_array.size ());

	size_t index = 0;
	for (const auto & batch : m_batch_array) {
		size_t gis  = batch->m_geometry_indices.size ();
		size_t ges  = batch->m_geometry_entries.size () * sizeof (Batch::GeometryEntry);
		size_t cmds = batch->m_commands.size () * sizeof (DrawCommand);

		size_t mis  = batch->m_mesh_indices.size ();
		size_t mes  = batch->m_mesh_entries.size () * sizeof (Batch::MeshEntry);
		size_t inss = batch->m_instances.size () * sizeof (InstanceData); 
		printf ("\t[%llu]\t{GIS: %llu elements; GES: %llu bytes; CMDS: %llu bytes}\n\t\t{MIS: %llu elements; MES: %llu bytes; INSS: %llu bytes}\n", 
				index,
				gis, ges, cmds,
				mis, mes, inss
			);

		printf ("\t\t{MCnt: %u; VBS: %u; IBS: %u}\n", batch->m_mesh_count, batch->m_vertex_count, batch->m_index_count);

		++index;
	}

	printf ("--------------------------------------------------------------------------------------------------\n");
}