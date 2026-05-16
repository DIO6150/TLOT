#include <Renderer/Renderables/Model.hpp>

using namespace TLOT;

void Model::SetDiffuseTexture (size_t index, ResourceHandle texture)
{
	assert (alive);
	
	if (index >= material.diffuseTextures.size ())
	{
		material.diffuseTextures.push_back (texture);
	}
	else
	{
		material.diffuseTextures[index] = texture;
	}

	renderer->UpdateInstanceMaterial (instance, material);
}

glm::mat4 Model::GetModelMatrix () const
{
	return transform.GetModelMatrix ();
}

glm::vec3 Model::GetPosition () const
{
	return transform.position;
}

glm::vec3 Model::GetEffectivePosition () const
{
	return transform.position - transform.pivot;
}

glm::quat Model::GetRotation () const
{
	return transform.rotation;
}

glm::vec3 Model::GetScale () const
{
	return transform.scale;
}

void Model::SetPosition (glm::vec3 position)
{
	assert (alive);
	transform.position = position;

	renderer->UpdateInstanceTransform (instance, transform);
}

void Model::SetPivot (glm::vec3 pivot)
{
	assert (alive);
	transform.pivot = pivot;

	renderer->UpdateInstanceTransform (instance, transform);
}

void Model::SetRotation (glm::quat rotation)
{
	assert (alive);
	transform.rotation = rotation;

	renderer->UpdateInstanceTransform (instance, transform);
}

void Model::SetRotation (glm::vec3 eulerAngle)
{
	assert (alive);

	//glm::quat rotX = glm::quat {{1.0, 0.0, 0.0}, eulerAngle.x};
	//glm::quat rotY = glm::quat {{0.0, 1.0, 0.0}, eulerAngle.y};
	//glm::quat rotZ = glm::quat {{0.0, 0.0, 1.0}, eulerAngle.z};
	//glm::quat finalRot = rotX * rotY * rotZ;

	glm::quat finalRot = glm::quat (eulerAngle);
	transform.rotation = finalRot;

	renderer->UpdateInstanceTransform (instance, transform);
}

void Model::SetScale (glm::vec3 scale)
{
	assert (alive);
	transform.scale = scale;

	renderer->UpdateInstanceTransform (instance, transform);
}

void Model::Translate (glm::vec3 translate)
{
	assert (alive);
	transform.position += translate;

	renderer->UpdateInstanceTransform (instance, transform);
}

void Model::Rotate (glm::quat rotation)
{
	assert (alive);
	transform.rotation = rotation * transform.rotation;

	renderer->UpdateInstanceTransform (instance, transform);
}

void Model::Scale (glm::vec3 scale)
{
	assert (alive);
	transform.scale += scale;

	renderer->UpdateInstanceTransform (instance, transform);
}

InstanceID Model::GetInstance ()
{
	assert (alive);
	return instance;
}

void Model::Kill ()
{
	renderer->UnregisterInstance (instance);
	renderer = nullptr;
	alive = false;
}
