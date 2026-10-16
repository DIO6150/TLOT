#pragma once

#include <TetoEngine/Private/Renderer/MaterialTemplate.hpp>

namespace TetoEngine
{
	class MaterialTemplateSTD430: public MaterialTemplate
	{
		size_t getSize(UniformType type) override;
		size_t getAlignment(UniformType type) override;
	};
}
