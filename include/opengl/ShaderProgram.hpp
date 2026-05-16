#pragma once

#include <unordered_map>
#include <string>

#include <Core/Resource.hpp>

#include <glm/glm.hpp>

namespace TLOT
{
	class ShaderProgram
	{
	public:
		ShaderProgram ();

		void CompileVertex (std::string source);
		void CompileFragment (std::string source);

		void Link ();

		void Use () const;
		void Cancel () const;

		// TODO-add : cache system
		void UploadMatrix4	(std::string key, const glm::mat4& matrix) const;
		void Upload1i		(std::string key, int i) const;
		void Upload3fv		(std::string key, glm::vec3 vec) const;
		void Upload1f		(std::string key, float f) const;

	private:	
		uint32_t m_program = 0;

		uint32_t m_vertexShader = 0;
		uint32_t m_fragmentShader = 0;
	};
}
