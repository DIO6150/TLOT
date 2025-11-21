#pragma once

#include <initializer_list>
#include <string>
#include <unordered_map>
#include <stdint.h>

namespace Engine
{
	enum ShaderType {
		VERTEX_SHADER,
		FRAGMENT_SHADER
	};

	class Shader {
	private:
		uint32_t m_object;
		std::unordered_map <ShaderType, std::string>  m_paths;

	public:
		Shader ();
		Shader (std::initializer_list <std::string> paths);

		void Use ();
	}; 
}
