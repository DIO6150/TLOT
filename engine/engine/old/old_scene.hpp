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
		std::vector<ED::Batch>		m_batches;


		ED::Batch *	GetBatch 	(Shader *shader);
		ED::Batch *	CreateBatch  	(Shader* shader_base);

		
		
		friend GameObject;

	public:
		Scene ();
		
		GameObject *	CreateObject (Mesh *mesh, ED::InstanceData data, Shader *shader);
		void		Render ();

	};
}