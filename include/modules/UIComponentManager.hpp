#pragma once

#include <concepts>

#include <modules/UIComponent.hpp>

#include <modules/AssetManager.hpp>


namespace TLOT
{
	class UIComponentManager
	{
		template<std::derived_from<UIComponent> Component, typename ... Args>
		Component GenerateComponent (Args && ... args)
		{
			Component component {std::forward<Args> (args)};

			for (auto & [animation, texture] : component.RequestTextures ())
			{
				ResourceHandle textureHandle = m_assetManager.GetTextureID (texture);
				component.m_sprites[animation].textures.push_back (textureHandle);
			}

			return component;
		}

	private:
		AssetManager & m_assetManager;
	};
}