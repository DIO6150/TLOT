#pragma once

#include <memory>

#include <Renderer/Renderables/Model.hpp>
#include <Renderer/Renderables/Sprite.hpp>
#include <Renderer/RendererCommon.hpp>
#include <Renderer/IRenderer.hpp>

#include <Core/ResourceHandle.hpp>

namespace TLOT
{
	class ModelFactory
	{
	public:
		static Model CreateModel (IRenderer & renderer, Mesh const & mesh, Transform transform);
		static Model CreateModel (IRenderer & renderer, Mesh const & mesh);

		static Sprite CreateSprite (IRenderer & renderer);
	};
}