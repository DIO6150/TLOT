#pragma once

#include <resources/Resource.hpp>

#include <opengl/TextureAtlas.hpp>
#include <core/Logger.hpp>

namespace TLOT
{
	struct alignas(16) TextureOffset
	{
		float x, y, w, h, d;

		TextureOffset (ResourceHandle handle, TextureAtlas & atlas)
		{
			TextureQuad const & textureQuad = atlas.Quad (handle);

			x = textureQuad.x / atlas.Width  ();
			w = textureQuad.w / atlas.Width  ();
			y = textureQuad.y / atlas.Height ();
			h = textureQuad.h / atlas.Height ();
			d = static_cast<float> (textureQuad.depth);

			Logger::log (LogLevel::Info, "{} : Offset = ({}, {}, {}, {}, {})", handle, x, y, w, h, d);
		}
	};
}
