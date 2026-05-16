#pragma once

#include <set>
#include <map>

#include <Core/ResourceHandle.hpp>

#include <Resources/Texture.hpp>

#include <Renderer/IRenderer.hpp>

namespace TLOT
{
	class SceneEditor
	{
	public:
		void RegisterRenderer (IRenderer & renderer);

		void OpenMenu ();

	private:
		uint32_t GetOpenGLTexture (ResourceHandle handle, Texture const & texture);

		std::map<ResourceHandle, uint32_t> m_textures;
		std::set<IRenderer *> m_renderers;
	};
}