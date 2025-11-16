#pragma once

#include <string>

#include <stdint.h>

namespace ED {
	enum class ShaderType {
		VERTEX, FRAGMENT
	};

	class Shader {
	public:
		Shader () : type (ShaderType::VERTEX) {}
		Shader (const std::string & url, const ShaderType && type);
		

		const ShaderType type;

	private:	
		uint32_t	m_handle;
		
		std::string 	m_url;
	};
}