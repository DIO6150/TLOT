#pragma once


#include <string>
#include <unordered_map>
#include <vector>

#include <stdint.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <details/batch.hpp>
#include <details/geometry.hpp>
#include <details/material.hpp>
#include <details/mesh.hpp>
#include <details/shader.hpp>
#include <details/texture.hpp>

#include <scene.hpp>
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

		Handle createMaterial ();

		Scene * createScene();

		void render () const;

		EngineDebugStat stat;

	private:
		ResourceManager<ED::Shader>	m_shader;
		ResourceManager<ED::Texture>	m_texture;
		ResourceManager<ED::Geometry>	m_geometry;
		ResourceManager<ED::Material>	m_material;

		std::vector<Scene>		m_scenes;
	};
}