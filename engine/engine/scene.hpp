#pragma once

#include <vector>
#include <unordered_map>
#include <stdint.h>

#include <game_object.hpp>
#include <mesh.hpp>
#include <shader.hpp>

#include <details/batch.hpp>
#include <details/instance_data.hpp>


namespace Engine {


	class Scene {
	private:
		std::vector<GameObject> 			m_loaded_objects;
		std::vector<EngineDetail::Batch>		m_batches;


		EngineDetail::Batch *	GetBatch 	(Shader *shader);
		EngineDetail::Batch *	CreateBatch  	(Shader* shader_base);

		
		
		friend GameObject;

	public:
		Scene ();
		
		GameObject *	CreateObject (Mesh *mesh, EngineDetail::InstanceData data, Shader *shader);
		void		Render ();

	};
}