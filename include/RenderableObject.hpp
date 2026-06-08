#pragma once

#include <Material/MaterialInstance.hpp>
#include <Renderer/Renderer.hpp>

namespace TLOT
{
	class IObject
	{
	public:
		virtual glm::mat4 GetTransformMatrix() const = 0;
		virtual glm::vec3 GetPosition() const = 0;
		virtual glm::quat GetRotation() const = 0;
		virtual glm::vec3 GetScale()    const = 0;

		virtual void SetPosition(glm::vec3 position) = 0;
		virtual void SetPivot(glm::vec3 pivot) = 0;
		virtual void SetRotation(glm::quat rotation) = 0;
		virtual void SetRotation(glm::vec3 eulerAngle) = 0;
		virtual void SetScale(glm::vec3 scale) = 0;

		virtual void Translate(glm::vec3 translate) = 0;
		virtual void Rotate(glm::quat rotation) = 0;
		virtual void Scale(glm::vec3 scale) = 0;
		virtual void Render() = 0;

		virtual SceneObject GetSceneObject() = 0;
		virtual MaterialInstance & GetMaterial() = 0;
		virtual Transform & GetTransform() = 0;

		virtual void UpdateTransform() = 0;
		virtual void UpdateMaterial() = 0;
	};

	class CommonObject : public IObject
	{
	public:
		CommonObject(Renderer * renderer, ResourceHandle Geometry, ResourceHandle materialTemplateHandle);

		glm::mat4 GetTransformMatrix() const override;
		glm::vec3 GetPosition() const override;
		glm::quat GetRotation() const override;
		glm::vec3 GetScale()    const override;

		void SetPosition(glm::vec3 position) override;
		void SetPivot(glm::vec3 pivot) override;
		void SetRotation(glm::quat rotation) override;
		void SetRotation(glm::vec3 eulerAngle) override;
		void SetScale(glm::vec3 scale) override;

		void Translate(glm::vec3 translate) override;
		void Rotate(glm::quat rotation) override;
		void Scale(glm::vec3 scale) override;
		void Render() override;

		SceneObject GetSceneObject() override;
		MaterialInstance & GetMaterial() override;
		Transform & GetTransform() override;

		void UpdateTransform() override;
		void UpdateMaterial() override;

	protected:
		SceneObject m_instance;
		Transform m_transform;
		MaterialInstance m_material;

		Renderer * m_renderer = nullptr;

		bool m_transformUpdateFlag = true;
		bool m_materialUpdateFlag = true;
	};
}
