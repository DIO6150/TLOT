
#include <algorithm>

#include <TetoEngine/Private/Renderer/MaterialTemplate.hpp>


using namespace TetoEngine;

void MaterialTemplate::addUniform(std::string key, UniformType type)
{
	size_t size      = getSize     (type);
	size_t alignment = getAlignment(type);

	size_t remainder = totalSize_ % alignment;
	size_t padding   = (remainder == 0) ? 0 : (alignment - remainder);

	size_t offset    = totalSize_ + padding;

	uniforms_.emplace_back(type, key, size, offset);

	totalSize_    = offset + size;
	maxAlignment_ = std::max(maxAlignment_, alignment);
}

std::vector<UniformLayout> const & MaterialTemplate::layout() const
{
	return uniforms_;
}

UniformLayout const * MaterialTemplate::uniform(std::string key) const
{
	for (auto & param : uniforms_)
	{
		if (param.name == key) return &param;
	}

	return nullptr;
}

size_t MaterialTemplate::size() const
{
	size_t remainder = totalSize_ % maxAlignment_;

	if (remainder == 0)
		return totalSize_;

	return totalSize_ + (maxAlignment_ - remainder);
}
