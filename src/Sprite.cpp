#include <modules/Sprite.hpp>

using namespace TLOT;

void SpriteAnimation::Step ()
{
	if (currentTexture == textures.size ())
		currentTexture = 0;

	currentTexture++;
}

void Sprite::Update (double deltaTime)
{
	SpriteAnimation & current = m_animations[m_currentAnimation];
	nextFrame -= deltaTime * current.speed;

	if (nextFrame > 0.0) return;

	SetDiffuseTexture (0, current.GetCurrentTexture (), m_assetManager);

	current.Step ();
	nextFrame = 1.0;
}

void Sprite::ForceUpdate ()
{
	SpriteAnimation & current = m_animations[m_currentAnimation];
	SetDiffuseTexture (0, current.GetCurrentTexture (), m_assetManager);
}

void Sprite::RegisterAnimation (std::string animationName, SpriteAnimation & animation)
{
	m_animations.emplace (animationName, animation);
}

void Sprite::SetAnimation (std::string animationName)
{
	if (m_animations.count (animationName) < 1) return;

	m_currentAnimation = animationName;
}

void Sprite::Pause ()
{
	m_animations[m_currentAnimation].playing = false;
}

void Sprite::Resume ()
{
	m_animations[m_currentAnimation].playing = true;
}

void Sprite::SetSpeed (double speed)
{
	m_animations[m_currentAnimation].speed = speed;
}
