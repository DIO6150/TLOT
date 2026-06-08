#pragma once

#include <stdint.h>

namespace TLOT
{
	struct TextureQuad
	{
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;
		float depth = 0;

		TextureQuad (float x, float y, float w, float h, float depth):
			x{x}, y{y}, w{w}, h{h}, depth{depth}
		{

		}
		TextureQuad () = default;
		
		bool operator== (TextureQuad & other);

		TextureQuad (TextureQuad & other)  = default;
		TextureQuad (TextureQuad && other) = default;

		TextureQuad & operator= (TextureQuad & other) = default;
		TextureQuad & operator= (TextureQuad const & other) = default;
		TextureQuad & operator= (TextureQuad && other) = default;
	};
}