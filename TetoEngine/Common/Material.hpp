#pragma once

#include <memory>
#include <string>

#include <TetoEngine/Common/ID.hpp>
#include <TetoEngine/Private/GPUResources/ShaderProgram.hpp>

#include <glm/glm.hpp>

namespace TetoEngine
{
	class MaterialTemplate;

	class Material
	{
	public:
		void setTexture(std::string key, glm::vec4 textureCoords);

		void setInstanceProperty(std::string key, int value);
		void setInstanceProperty(std::string key, float value); // im not sure compiler can do the difference between a float and an int
		void setInstanceProperty(std::string key, glm::vec2 value);
		void setInstanceProperty(std::string key, glm::vec3 value);
		void setInstanceProperty(std::string key, glm::vec4 value);
		void setInstanceProperty(std::string key, glm::mat4 value);

		void setShaderProperty(std::string key, int value);
		void setShaderProperty(std::string key, float value);
		void setShaderProperty(std::string key, glm::vec2 value);
		void setShaderProperty(std::string key, glm::vec3 value);
		void setShaderProperty(std::string key, glm::vec4 value);
		void setShaderProperty(std::string key, glm::mat4 value);

		std::vector<uint8_t> const & rawProperties() const;
		size_t const propertiesSize() const;

		Material() = default;
		
	private:
		Material(std::polymorphic<MaterialTemplate> & materialTemplate, ShaderProgram & program);

	private:
		std::polymorphic<MaterialTemplate> instanceTemplate_;
		ShaderProgram program_;

		std::vector<uint8_t> rawProperties_;

	private:
		friend class InstanceRenderer;
	};
}