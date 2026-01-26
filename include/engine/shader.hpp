#pragma once

#include <string>
#include <unordered_map>
#include <stdint.h>

#include <glm/glm.hpp>

namespace Engine {
	enum class ShaderType {
		VERTEX, FRAGMENT, NONE
	};

	class Shader {
	public:
		Shader ();
		Shader (const std::string & vertex_url, const std::string & frag_url);
		void createShader (const std::string & url, const ShaderType & type);
		void compile ();

		void use ();
		void cancel ();

		// TODO-add : cache system
		void uploadMatrix4	(std::string key, glm::mat4& matrix);
		void upload1i		(std::string key, int i);
		void upload3fv		(std::string key, glm::vec3 vec);

	private:	
		uint32_t					m_program;
		std::unordered_map<ShaderType, uint32_t>	m_shaders;
		std::unordered_map<ShaderType, std::string>	m_urls;
	};
}