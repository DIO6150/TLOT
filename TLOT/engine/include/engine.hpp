#pragma once


#include <string>
#include <unordered_map>
#include <vector>

#include <stdint.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <batch.hpp>
#include <geometry.hpp>
#include <material.hpp>
#include <mesh.hpp>
#include <scene.hpp>
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
		EngineDebugStat () {

		}

		void print () {
			
		}
	};

	class Engine {
	public:
		Engine ();
		~Engine ();

		Handle loadShader	(const std::string url, const ED::ShaderType type);
		Handle loadGeometry	(const std::string url);

		void   loadSceneAssimp	(const std::string url);

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

		GLFWwindow *			window;
	};
}