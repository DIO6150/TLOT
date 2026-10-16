
#include <TetoEngine/Common/Material.hpp>
#include <TetoEngine/Private/Renderer/MaterialTemplate.hpp>

#include <TetoEngine/Utils/Logger.hpp>

using namespace TetoEngine;

struct Material::Impl
{
	Impl(std::shared_ptr<MaterialTemplate> materialTemplate, ID64 handle)
		: template_ {materialTemplate}
		, handle_ {handle}
	{
		raw_.resize(template_->size());
	}

	template<class T>
	void setUniform(std::string key, T value)
	{
		auto param = template_.uniform(key);
	
		if (!param) return;
		if (sizeof(T) > param->size_) return;
	
		memcpy(raw_.data() + param->offset, &value, sizeof(T));
	}

	std::shared_ptr<MaterialTemplate> template_;
	ID64 handle_;
	std::vector<uint8_t> raw_;
};

Material::Material(std::shared_ptr<MaterialTemplate> materialTemplate, ID64 handle)
{
	impl_ = std::make_shared<Material::Impl>(materialTemplate, handle);
}

void Material::setTexture(std::string key, glm::vec4 textureCoords)
{
	impl_->setUniform<glm::vec4>(key, textureCoords);
}

void Material::setUniform1i(std::string key, int value)
{
	impl_->setUniform<int>(key, value);
}

void Material::setUniform1f(std::string key, float value)
{
	impl_->setUniform<float>(key, value);
}

void Material::setUniformVec2(std::string key, glm::vec2 value)
{
	impl_->setUniform<glm::vec2>(key, value);
}

void Material::setUniformVec3(std::string key, glm::vec3 value)
{
	impl_->setUniform<glm::vec3>(key, value);
}

void Material::setUniformVec4(std::string key, glm::vec4 value)
{
	impl_->setUniform<glm::vec4>(key, value);
}

void Material::setUniformMat4(std::string key, glm::mat4 value)
{
	impl_->setUniform<glm::mat4>(key, value);
}

std::vector<uint8_t> const & Material::raw() const
{
	return impl_->raw_;
}

size_t const Material::size() const
{
	return impl_->raw_.size();
}

ID64 Material::handle() const
{
	return impl_->handle_;
}