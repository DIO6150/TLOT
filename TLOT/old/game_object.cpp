#include <game_object.hpp>

#include <mesh.hpp>
#include <scene.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <scene.hpp>
#include <batch.hpp>

Engine::GameObject::GameObject (
	Mesh *				mesh, 
	ED::InstanceData 	data,
	Scene *				scene,
	ED::Batch *		batch
) :
	m_mesh		(mesh), 
	m_data		(data),
	m_parent_scene	(scene),
	m_parent_batch	(batch),
	m_should_render	(true) {

}

void Engine::GameObject::Translate (glm::vec3 translation) {
	m_data.matrix = glm::translate (m_data.matrix, translation);
	m_parent_batch->m_should_resend_ssbo = true;
}

void Engine::GameObject::Show () {
	if (m_should_render) return;

	++m_parent_batch->m_visible_mesh_count;
	++m_parent_batch->m_geometry_counter [m_mesh];
	m_should_render = true;
	m_parent_batch->m_should_resend_ssbo = true;
}

void Engine::GameObject::Hide () {
	if (!m_should_render) return;

	--m_parent_batch->m_visible_mesh_count;
	--m_parent_batch->m_geometry_counter [m_mesh];
	m_should_render = false;
	m_parent_batch->m_should_resend_ssbo = true;
}