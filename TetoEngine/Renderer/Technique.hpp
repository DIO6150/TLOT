#pragma once

#include <TetoEngine/Common/ID.hpp>
#include <TetoEngine/Renderer/TRID.hpp>

namespace TetoEngine
{
	enum class ProjectionMode
	{
		Perspective,
		Orthographic
	};

	struct Technique
	{
		enum class Arg
		{
			DEPTH_LESS,
			BLEND_SRC_ALPHA,
			BLEND_ONE_MINUS_SRC_ALPHA,
			FACE_CCW,
			FACE_CW,
		};

		ProjectionMode mode = ProjectionMode::Perspective;
		ID64 material = InvalidID;
		TRID program  = InvalidID;

		uint32_t activeCamera = -1;

		bool enableDepthTest = true;
		bool enableDepthMask = true;
		Arg depthTestFunction = Arg::DEPTH_LESS;

		bool enableBlend = true;
		Arg blendFunctionSource = Arg::BLEND_SRC_ALPHA;
		Arg blendFunctionDestination = Arg::BLEND_ONE_MINUS_SRC_ALPHA;

		Arg frontFace = Arg::FACE_CCW;

		bool useFontAtlas = false;
	};
}
