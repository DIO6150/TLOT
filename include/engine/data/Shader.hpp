#pragma once

#include <string>
#include <unordered_map>
#include <stdint.h>

#include <glm/glm.hpp>

namespace Engine::Data {
	enum class ShaderType {
		VERTEX, FRAGMENT, NONE
	};

	class Shader {
	public:
		Shader ();
		Shader (const std::string & vertex_url, const std::string & frag_url);
		void createShader (const std::string & url, const ShaderType & type);
		void compile ();

		void use () const;
		void cancel () const;

		// TODO-add : cache system
		void uploadMatrix4	(std::string key, const glm::mat4& matrix) const;
		void upload1i		(std::string key, int i) const;
		void upload3fv		(std::string key, glm::vec3 vec) const;
		void upload1f		(std::string key, float f) const;

	private:	
		uint32_t					m_program;
		std::unordered_map<ShaderType, uint32_t>	m_shaders;
		std::unordered_map<ShaderType, std::string>	m_urls;
	};
}