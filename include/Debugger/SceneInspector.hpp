#pragma once

#include <set>
#include <map>

#include <Core/ResourceHandle.hpp>
#include <Resources/Texture.hpp>
#include <Renderer/Renderer.hpp>

#include <RenderableObject.hpp>

namespace TLOT
{
	class SceneInspector
	{
	public:
		void OpenMenu();
		void PreCache();
		void SetRenderer(Renderer * renderer)
		{
			m_renderer = renderer;
		}

		
		void RegisterRenderableObject(IObject * object);

	private:
		uint32_t GetOpenGLTexture(ResourceHandle handle, Texture const & texture);
		void DrawSceneObjectWidget(IObject * renderable);


		std::map<ResourceHandle, uint32_t> m_textures;
		Renderer * m_renderer = nullptr;

		std::vector<IObject *> m_objectInfos;
	};
}