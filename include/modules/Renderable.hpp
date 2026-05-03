#pragma once

#include <core/Transform.hpp>
#include <core/Material.hpp>

#include <resources/Resource.hpp>

#include <modules/Renderer.hpp>

#include <glm/gtc/quaternion.hpp>

namespace TLOT
{
	struct Renderable
	{
	private:
		GeometryID geometry = 0;
		InstanceID instance = 0;

		Transform transform;
		Material material;

		Renderer * renderer = nullptr;
		bool alive = true;

		friend class RenderableManager;

	public:

		Renderable () = default;
		Renderable (Renderable && other) = default;
		Renderable (Renderable & other) = default;
		Renderable (Renderable const & other) = default;

		void SetDiffuseTexture (size_t index, ResourceHandle texture, AssetManager & assetManager)
		{
			assert (alive);
			if (index >= material.diffuseTextures.size ())
				material.diffuseTextures.push_back (texture);
			else
				material.diffuseTextures[index] = texture;

			renderer->UpdateInstanceMaterial (instance, material, assetManager);
		}

		glm::mat4 GetModelMatrix () const
		{
			return transform.GetModelMatrix ();
		}

		glm::vec3 GetPosition () const
		{
			return transform.position;
		}

		glm::vec3 GetEffectivePosition () const
		{
			return transform.position - transform.pivot;
		}

		glm::quat GetRotation () const
		{
			return transform.rotation;
		}

		glm::vec3 GetScale () const
		{
			return transform.scale;
		}

		void SetPosition (glm::vec3 position)
		{
			assert (alive);
			transform.position = position;

			renderer->UpdateInstanceTransform (instance, transform);
		}

		void SetPivot (glm::vec3 pivot)
		{
			assert (alive);
			transform.pivot = pivot;

			renderer->UpdateInstanceTransform (instance, transform);
		}

		void SetRotation (glm::quat rotation)
		{
			assert (alive);
			transform.rotation = rotation;

			renderer->UpdateInstanceTransform (instance, transform);
		}

		void SetScale (glm::vec3 scale)
		{
			assert (alive);
			transform.scale = scale;

			renderer->UpdateInstanceTransform (instance, transform);
		}

		void Translate (glm::vec3 translate)
		{
			assert (alive);
			transform.position += translate;

			renderer->UpdateInstanceTransform (instance, transform);
		}

		void Rotate (glm::quat rotation)
		{
			assert (alive);
			transform.rotation = rotation * transform.rotation;

			renderer->UpdateInstanceTransform (instance, transform);
		}

		void Scale (glm::vec3 scale)
		{
			assert (alive);
			transform.scale += scale;

			renderer->UpdateInstanceTransform (instance, transform);
		}

		InstanceID GetInstance ()
		{
			assert (alive);
			return instance;
		}

		void Kill ()
		{
			renderer->UnregisterInstance (instance);
			renderer = nullptr;
			alive = false;
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
