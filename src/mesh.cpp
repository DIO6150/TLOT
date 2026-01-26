#include <engine/mesh.hpp>
#include <engine/scene.hpp>

Engine::Mesh::Mesh () : 
Mesh (
	nullptr, 
	nullptr,
	glm::vec3 {0.0},
	glm::vec3 {0.0},
	glm::vec3 {1.0},
	nullptr
) {

}

Engine::Mesh::Mesh (
			Scene *		scene,
			Geometry *	geometry,
			glm::vec3	position,
			glm::vec3	rotation,
			glm::vec3	scale,
			Material *   	material
		) : 
	visible 	{true},
	geometry 	{geometry},
	m_position	{position},
	m_rotation	{rotation},
	m_scale		{scale},
	p_scene 	{scene},
	material	{material}
{

}

void Engine::Mesh::translate (glm::vec3 offset) {
	m_position += offset;

	Batch * batch = p_scene->m_mesh_location[this];
	batch->updateInstance (this);
}

void Engine::Mesh::rotate (glm::vec3 offset) {
	m_rotation += offset;

	Batch * batch = p_scene->m_mesh_location[this];
	batch->updateInstance (this);
}

void Engine::Mesh::scale (glm::vec3 offset) {
	m_scale += offset;

	Batch * batch = p_scene->m_mesh_location[this];
	batch->updateInstance (this);
}