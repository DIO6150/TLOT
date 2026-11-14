#pragma once

#include <set>
#include <map>

#include <Core/ID64.hpp>
#include <Resources/Texture.hpp>
#include <Renderer/Renderer.hpp>

#include <RenderableObject.hpp>

namespace TetoEngine
{
	class SceneInspector
	{
	public:
		void OpenMenu();
		void PreCache();
		void SetRenderer(InstanceRenderer * renderer)
		{
			renderer_ = renderer;
		}

		
		void RegisterRenderableObject(IObject * object);

	private:
		uint32_t GetOpenGLTexture(ID64 handle, Texture const & texture);
		void DrawSceneObjectWidget(IObject * renderable);


		std::map<ID64, uint32_t> m_textures;
		InstanceRenderer * renderer_ = nullptr;

		std::vector<IObject *> m_objectInfos;
	};
}