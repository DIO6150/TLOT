#pragma once

#include <engine/utils/Types.hpp>

#include <engine/core/ResourceManager.hpp>

#include <engine/data/ShaderInputs.hpp>

#include <stdint.h>

namespace Engine::Internal {
	class PostProcessingEffect {
	public:
		PostProcessingEffect (Core::HandleID shader, size_t output);
		PostProcessingEffect (Core::HandleID shader, size_t output, Data::ShaderInputs && params);

		Core::HandleID postProcess;
		bool active;
		size_t colorAttachmentCount;
		Data::ShaderInputs params;
	};
}