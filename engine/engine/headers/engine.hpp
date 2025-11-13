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
	class Engine {
	public:
		Engine ();

		Handle loadShader	(const std::string url, const ED::ShaderType type);
		Handle loadGeometry	(const std::string url);

		Handle createMesh	(ED::Geometry & geometry);
		void   removeMesh	(Handle mesh);

		void render () const;

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