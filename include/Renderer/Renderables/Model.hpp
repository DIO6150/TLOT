#pragma once

#include <memory>

#include <Transform.hpp>

#include <Core/Resource.hpp>

#include <MeshData/Material.hpp>

#include <Renderer/IRenderer.hpp>
#include <Renderer/RendererCommon.hpp>

#include <glm/gtc/quaternion.hpp>

namespace TLOT
{
	struct Model
	{
	public:

		void SetDiffuseTexture (size_t index, ResourceHandle texture);

		glm::mat4 GetModelMatrix () const;
		glm::vec3 GetPosition () const;
		glm::vec3 GetEffectivePosition () const;
		glm::quat GetRotation () const;
		glm::vec3 GetScale () const;

		void SetPosition (glm::vec3 position);
		void SetPivot (glm::vec3 pivot);
		void SetRotation (glm::quat rotation);
		void SetRotation (glm::vec3 eulerAngle);
		void SetScale (glm::vec3 scale);

		void Translate (glm::vec3 translate);
		void Rotate (glm::quat rotation);
		void Scale (glm::vec3 scale);

		InstanceID GetInstance ();

		void Kill ();


		Model (Model & other) = default;
		Model (Model const & other) = default;

		Model () = default;
		Model (Model && other) = default;
		
	private:
		GeometryID geometry = 0;
		InstanceID instance = 0;

		Transform transform;
		Material material;

		IRenderer * renderer = nullptr;
		bool alive = true;

		friend class ModelFactory;
	};
}
