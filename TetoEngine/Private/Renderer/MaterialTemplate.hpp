#pragma once

#include <TetoEngine/Private/Renderer/IMaterialTemplate.hpp>

namespace TetoEngine
{
	class MaterialTemplate: public IMaterialTemplate
	{
	public:
		void addProperty(std::string key, UniformType type);

		std::vector<UniformLayout> const & layout() const;
		std::optional<UniformLayout> property(std::string key) const;
		size_t size() const;

	private:
		std::vector<UniformLayout> uniforms_;
		size_t totalSize_    = 0;
		size_t maxAlignment_ = 1;
	};
}