#pragma once

#include <vector>
#include <unordered_map>
#include <stdint.h>

#include <game_object.hpp>
#include <mesh.hpp>
#include <shader.hpp>

#include <batch.hpp>
#include <instance_data.hpp>


namespace Engine {


	class Scene {
	private:
		std::vector<GameObject> 			m_loaded_objects;
		std::vector<Engine::Batch>		m_batches;


		Engine::Batch *	GetBatch 	(Shader *shader);
		Engine::Batch *	CreateBatch  	(Shader* shader_base);

		
		
		friend GameObject;

	public:
		Scene ();
		
		GameObject *	CreateObject (Mesh *mesh, Engine::InstanceData data, Shader *shader);
		void		Render ();

	};
}