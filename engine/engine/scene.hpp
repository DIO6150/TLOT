#pragma once

#include <vector>
#include <unordered_map>
#include <stdint.h>

#include <game_object.hpp>
#include <mesh.hpp>
#include <batch.hpp>
#include <shader.hpp>

#include <details/instance_data.hpp>


namespace Engine {


	class Scene {
	private:
		std::vector<GameObject> 	m_loaded_objects;
		std::vector<Batch>		m_batches;
		


		Batch *	GetBatch (Shader *shader);

		
		
		friend GameObject;

	public:
		Scene ();
		
		Batch *		CreateBatch  (Shader* shader_base);
		GameObject *	CreateObject (Mesh *mesh, EngineDetail::InstanceData data, Shader *shader);
		void		Render ();

	};
}