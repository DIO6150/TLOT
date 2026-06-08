#include <RenderableObject.hpp>

using namespace TLOT;

CommonObject::CommonObject(Renderer * renderer, ResourceHandle geometry, ResourceHandle materialTemplateHandle)
	: m_material{AssetManager::CreateMaterial(materialTemplateHandle)}
	, m_renderer{renderer}
{
	m_instance = renderer->Instanciate(geometry, m_material, m_transform);
}

glm::mat4 CommonObject::GetTransformMatrix() const
{
	return m_transform.GetModelMatrix();
}

glm::vec3 CommonObject::GetPosition() const
{
	return m_transform.position;
}

glm::quat CommonObject::GetRotation() const
{
	return m_transform.rotation;
}

glm::vec3 CommonObject::GetScale() const
{
	return m_transform.scale;
}

void CommonObject::SetPosition(glm::vec3 position)
{
	m_transformUpdateFlag = true;
	m_transform.position = position;
}

void CommonObject::SetPivot(glm::vec3 pivot)
{
	m_transformUpdateFlag = true;
	m_transform.pivot = pivot;
}

void CommonObject::SetRotation(glm::quat rotation)
{
	m_transformUpdateFlag = true;
	m_transform.rotation = rotation;
}

void CommonObject::SetRotation(glm::vec3 eulerAngle)
{
	m_transformUpdateFlag = true;
	glm::quat finalRot = glm::quat(eulerAngle);
	m_transform.rotation = finalRot;
}

void CommonObject::SetScale(glm::vec3 scale)
{
	m_transformUpdateFlag = true;
	m_transform.scale = scale;
}

void CommonObject::Translate(glm::vec3 translate)
{
	m_transformUpdateFlag = true;
	m_transform.position += translate;
}

void CommonObject::Rotate(glm::quat rotation)
{
	m_transformUpdateFlag = true;
	m_transform.rotation = rotation * m_transform.rotation;
}

void CommonObject::Scale(glm::vec3 scale)
{
	m_transformUpdateFlag = true;
	m_transform.scale += scale;
}

void CommonObject::Render()
{
	if (m_transformUpdateFlag)
		m_renderer->Update(m_instance, m_transform);

	if (m_materialUpdateFlag)
		m_renderer->Update(m_instance, m_material);

	m_renderer->Render(m_instance);

	m_transformUpdateFlag = false;
	m_materialUpdateFlag = false;
}

SceneObject CommonObject::GetSceneObject()
{
	return m_instance;
}

MaterialInstance & CommonObject::GetMaterial()
{
	return m_material;
}

Transform & CommonObject::GetTransform()
{
	return m_transform;
}

void CommonObject::UpdateTransform()
{
	m_transformUpdateFlag = true;
}

void CommonObject::UpdateMaterial()
{
	m_materialUpdateFlag = true;
}
