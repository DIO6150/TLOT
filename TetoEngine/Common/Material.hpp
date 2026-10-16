#pragma once

#include <memory>
#include <string>

#include <TetoEngine/Common/ID.hpp>

#include <glm/glm.hpp>

namespace TetoEngine
{
	class MaterialTemplate;

	class Material
	{
	public:
		void setTexture(std::string key, glm::vec4 textureCoords);

		void setUniform1i(std::string key, int value);
		void setUniform1f(std::string key, float value);

		void setUniformVec2(std::string key, glm::vec2 value);
		void setUniformVec3(std::string key, glm::vec3 value);
		void setUniformVec4(std::string key, glm::vec4 value);
		void setUniformMat4(std::string key, glm::mat4 value);

		std::vector<uint8_t> const & raw() const;
		size_t const size() const;

		Material() = default;
		
	private:
		Material(std::shared_ptr<MaterialTemplate> materialTemplate, ID64 handle);
		ID64 handle() const;

	private:
		struct Impl;
		std::shared_ptr<Impl> impl_;

		friend class Renderer;
	};
}