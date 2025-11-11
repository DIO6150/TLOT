#include "mesh_manager.hpp"

#include <iostream>

Engine::MeshManager::MeshManager () {

}

Engine::Mesh *Engine::MeshManager::CreateMesh (
	std::vector<Engine::Vertex> vertices,
	std::vector<uint32_t> indices) {
	m_loaded_meshes.push_back (Mesh {});

	Mesh &m = m_loaded_meshes.back ();
	m.vertices = std::move (vertices);
	m.indices = std::move (indices);

	return (&m);
}

void Engine::MeshManager::Cleanup () {
	
}