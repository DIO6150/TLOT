
#include <TetoEngine/Common/Material.hpp>
#include <TetoEngine/Private/Renderer/MaterialTemplate.hpp>

#include <TetoEngine/Utils/Logger.hpp>

using namespace TetoEngine;

template<class T>
inline void setProperty(std::string key, T value, std::polymorphic<MaterialTemplate> const & instanceTemplate, uint8_t * instanceRaw)
{
	auto param = instanceTemplate.property(key);

	if (!param) return;
	if (sizeof(T) > param->size_) return;

	memcpy(instanceRaw.data() + param->offset, &value, sizeof(T));
}

Material::Material(std::polymorphic<MaterialTemplate> & materialTemplate, ShaderProgram & program)
	: instanceTemplate_{std::move(materialTemplate)}
	, program_         {program}
{
	
}

void Material::setTexture(std::string key, glm::vec4 textureCoords)
{
	setProperty<glm::vec4>(key, textureCoords, instanceTemplate_, rawProperties_.data());
}

void Material::setInstanceProperty(std::string key, int value)
{
	setProperty<int>(key, value, instanceTemplate_, rawProperties_.data());
}

void Material::setInstanceProperty(std::string key, float value)
{
	setProperty<float>(key, value, instanceTemplate_, rawProperties_.data());
}

void Material::setInstanceProperty(std::string key, glm::vec2 value)
{
	setProperty<glm::vec2>(key, value, instanceTemplate_, rawProperties_.data());
}

void Material::setInstanceProperty(std::string key, glm::vec3 value)
{
	setProperty<glm::vec3>(key, value, instanceTemplate_, rawProperties_.data());
}

void Material::setInstanceProperty(std::string key, glm::vec4 value)
{
	setProperty<glm::vec4>(key, value, instanceTemplate_, rawProperties_.data());
}

void Material::setInstanceProperty(std::string key, glm::mat4 value)
{
	setProperty<glm::mat4>(key, value, instanceTemplate_, rawProperties_.data());
}

void Material::setShaderProperty(std::string key, int value)
{

}

void Material::setShaderProperty(std::string key, float value)
{

}

void Material::setShaderProperty(std::string key, glm::vec2 value)
{

}

void Material::setShaderProperty(std::string key, glm::vec3 value)
{

}

void Material::setShaderProperty(std::string key, glm::vec4 value)
{

}

void Material::setShaderProperty(std::string key, glm::mat4 value)
{

}


std::vector<uint8_t> const & Material::rawProperties() const
{
	return rawProperties_;
}

size_t const Material::propertiesSize() const
{
	return rawProperties_.size();
}
