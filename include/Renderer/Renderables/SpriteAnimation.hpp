#pragma once

#include <vector>

#include <Core/ResourceHandle.hpp>

namespace TLOT
{
	struct SpriteAnimation
	{
		SpriteAnimation () = default;
		SpriteAnimation (std::vector<ResourceHandle> textures): textures {textures} {}

		ResourceHandle GetCurrentTexture ()
		{
			return textures[currentTexture];
		}

		std::vector<ResourceHandle> textures;
		size_t currentTexture = 0;
		double speed = 1.0;
		bool playing = false;

		void Step ();
	};
};
