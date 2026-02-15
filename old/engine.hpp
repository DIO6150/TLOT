#pragma once


#include <string>
#include <unordered_map>
#include <vector>

#include <stdint.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <engine/batch.hpp>
#include <engine/geometry.hpp>
#include <engine/material.hpp>
#include <engine/mesh.hpp>
#include <engine/shader.hpp>
#include <engine/texture.hpp>

#include <engine/camera.hpp>
#include <engine/scene.hpp>

namespace Engine {
	class Engine {
	public:
		Engine ();
		~Engine ();

		Shader   * loadShader		(const std::string & name, const std::string & vertex_url, const std::string & frag_url);
		Geometry * loadGeometry		(const std::string & name, const std::string url);
		Material * createMaterial	(const std::string & name, Shader * shader, Texture * diffuse);
		Geometry * createGeometry	(const std::string & name, std::vector<Vertex> vertices, std::vector<uint32_t> indices);
		Texture  * loadTexture		(const std::string & name, const std::string url, TexType type);
		
		Scene * createScene();
		Scene * loadSceneAssimp	(const std::string url);

		void render (Scene * scene);

		GLFWwindow * 	getWindow ();
		Camera & 	getCamera ();

	private:
		Map<std::unique_ptr<Shader>>	m_shader;
		Map<std::unique_ptr<Material>>	m_material;
		Map<std::unique_ptr<Texture>>	m_texture;
		Map<std::unique_ptr<Geometry>>	m_geometry;

		std::vector<Scene>		m_scenes;

		GLFWwindow *			m_window;
		Camera				m_camera;
		TextureAtlas			m_atlas;

		friend Scene;
	};
}