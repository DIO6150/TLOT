#pragma once

#include <resources/Resource.hpp>

#include <modules/AssetManager.hpp>
#include <modules/Renderable.hpp>

#include <string>
#include <map>

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
		bool playing = true;

		void Step ();
	};

	class Sprite: public Renderable
	{
	public:
		Sprite (Renderable base, AssetManager & assetManager): Renderable {base}, m_assetManager {assetManager} {}
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

		AssetManager & m_assetManager;
	};
}