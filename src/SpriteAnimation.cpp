#include <Renderer/Renderables/SpriteAnimation.hpp>

using namespace TLOT;

void SpriteAnimation::Step ()
{
	if (currentTexture == textures.size ())
		currentTexture = 0;

	currentTexture++;
}