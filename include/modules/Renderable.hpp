#pragma once

#include <core/Transform.hpp>
#include <core/Material.hpp>

#include <resources/Resource.hpp>

#include <modules/Renderer.hpp>

namespace TLOT
{
	struct Renderable
	{
	private:
		GeometryID geometry;
		InstanceID instance;

		Transform transform;
		Material material;

		Renderer * renderer;

		friend class RenderableManager;

	public:
		void SetDiffuseTexture (size_t index, ResourceHandle texture, AssetManager & assetManager)
		{
			if (index >= material.diffuseTextures.size ())
				material.diffuseTextures.push_back (texture);
			else
				material.diffuseTextures[index] = texture;

			renderer->UpdateInstanceMaterial (instance, material, assetManager);
		}
	};

	class RenderableManager
	{
	public:
		RenderableManager (AssetManager & assetManager, Renderer & renderer):
			m_assetManager {assetManager},
			m_renderer {renderer}
		{

		}

		Renderable Create (GeometryID geometry, Transform transform, Material const & material)
		{
			InstanceID instance = m_renderer.RegisterInstance (geometry, transform, material, m_assetManager);

			Renderable value;
			value.geometry  = geometry;
			value.instance  = instance;
			value.material  = material;
			value.transform = transform;
			value.renderer  = &m_renderer;

			return value;
		}

		Renderable Create (Mesh const & mesh, Transform transform)
		{
			GeometryID geometry = m_renderer.RegisterGeometry (mesh);
			return Create (geometry, transform, mesh.material);
		}

		Renderer & GetRenderer () { return m_renderer; }
		AssetManager & GetAssetManager () { return m_assetManager; }

	private:
		AssetManager & m_assetManager;
		Renderer & m_renderer;
	};
}
