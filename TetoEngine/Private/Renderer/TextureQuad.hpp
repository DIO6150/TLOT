#pragma once

#include <stdint.h>

namespace TetoEngine
{
	struct TextureQuad
	{
		float x     = 0.0f;
		float y     = 0.0f;
		float w     = 0.0f;
		float h     = 0.0f;
		float depth = 0.0f;
		
		TextureQuad () = default;
		TextureQuad (float x, float y, float w, float h, float depth);
		
		bool operator== (TextureQuad & other);
	};
}
