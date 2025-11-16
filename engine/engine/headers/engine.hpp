#pragma once


#include <string>
#include <unordered_map>
#include <vector>

#include <stdint.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <batch.hpp>
#include <geometry.hpp>
#include <mesh.hpp>
#include <shader.hpp>
#include <texture.hpp>
#include <resource_manager.hpp>

/*
	Render Data

	Shader Code

	Post Processing (FrameBuffer pipeline)

*/


namespace Engine {
	struct EngineDebugStat {
		uint32_t failed_removal;

		EngineDebugStat () : 
			failed_removal (0)
		{

		}

		void print () {
			std::cout << "Failed to remove " << failed_removal << " mesh(es)\n";
		}
	};

	class Engine {
	public:
		Engine ();

		Handle loadShader	(const std::string url, const ED::ShaderType type);
		Handle loadGeometry	(const std::string url);

		Handle createMesh	(Handle geometry_handle);
		void   removeMesh	(Handle mesh);

		void render () const;

		EngineDebugStat stat;

	private:
		ResourceManager<ED::Shader>	m_shader;
		ResourceManager<ED::Texture>	m_texture;
		ResourceManager<ED::Geometry>	m_geometry;
		// ResourceManager<Animation>	m_animation;

		ResourceManager<ED::Mesh>	m_mesh;

		
		std::vector<ED::InstanceData>	m_data_array;
		std::vector<ED::Batch>		m_batch_array;
		
		std::unordered_map<ED::Mesh *, ED::Batch *> m_mesh_location;
	};
}