#pragma once

#include <Renderer/RendererCommon.hpp>

namespace TLOT
{
	struct Technique
	{
		ProjectionMode mode = ProjectionMode::Perspective;
		ID64_t program = 0;
		ResourceHandle material = InvalidResource;

		uint32_t activeCamera = -1;

		bool enableDepthTest = true;
		GLenum depthTestFunction = GL_LESS;

		bool enableBlend = true;
		GLenum blendFunctionSource = GL_SRC_ALPHA;
		GLenum blendFunctionDestination = GL_ONE_MINUS_SRC_ALPHA;

		GLenum frontFace = GL_CCW;
	};
}
