#pragma once

#include <AssetManager.hpp>

#include <Core/ResourceHandle.hpp>

#include <Renderer/Renderables/Model.hpp>
#include <Renderer/Renderables/SpriteAnimation.hpp>

#include <string>
#include <map>

namespace TLOT
{	
	class Sprite: public Model
	{
	public:
		Sprite (Model && base): Model {std::move (base)} {}
		void Update (double deltaTime);
		void ForceUpdate ();

		void RegisterAnimation (std::string animationName, SpriteAnimation & animation);
		void SetAnimation (std::string animationName);

		void Pause  ();
		void Resume ();

		void SetSpeed (double speed);

	private:
		std::map<std::string, SpriteAnimation> m_animations;
		std::string m_defaultAnimation;
		std::string m_currentAnimation;

		double nextFrame = 1.0;
	};
}