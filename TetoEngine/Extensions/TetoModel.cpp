
#include <TetoEngine/Extensions/TetoModel.hpp>
#include <TetoEngine/Application/Application.hpp>

using namespace TetoEngine;

void TetoModel::init(Application * app)
{
	app_ = app;
}

void TetoModel::destroy()
{
	for (auto mesh : meshList_)
	{
		app_->killInstance(mesh);
	}

	meshList_     .clear();
	transformList_.clear();
	materialList_ .clear();

	dirtyTransform_ = true;
	dirtyMaterial_  = true;

	parent_ = nullptr;
}

void TetoModel::draw()
{
	if (!visible_) return;

	glm::mat4 computed = matrix();

	size_t index = 0;
	for (auto mesh : meshList_)
	{
		// branch prediction should make this fast
		if (dirtyTransform_) app_->updateInstance(mesh, computed * transformList_.at(index).model());
		if (dirtyMaterial_)  app_->updateInstance(mesh, materialList_.at(index));

		app_->drawInstance(mesh);
		++index;
	}

	dirtyMaterial_  = false;
	dirtyTransform_ = false;
}

glm::mat4 TetoModel::matrix() const
{
	if (parent_)
	{
		return parent_->transform_.model() * transform_.model();
	}

	return transform_.model();
}

glm::vec3 TetoModel::position() const
{
	return transform_.position;
}

glm::quat TetoModel::rotation() const
{
	return transform_.rotation;
}

glm::vec3 TetoModel::scale() const
{
	return transform_.scale;
}

bool TetoModel::isVisible() const
{
	return visible_;
}

Transform TetoModel::transform() const
{
	return transform_;
}

void TetoModel::setPosition(glm::vec3 position)
{
	transform_.position = position;
	updateTransform();
}

void TetoModel::setRotation(glm::quat rotation)
{
	transform_.rotation = rotation;
	updateTransform();
}

void TetoModel::setRotation(glm::vec3 eulerAngle)
{
	glm::quat finalRot = glm::quat(eulerAngle);
	transform_.rotation = finalRot;
	updateTransform();
}

void TetoModel::setScale(glm::vec3 scale)
{
	transform_.scale = scale;
	updateTransform();
}

void TetoModel::translate(glm::vec3 translation)
{
	transform_.position += translation;
	updateTransform();
}

void TetoModel::rotate(glm::quat rotation)
{
	transform_.rotation = rotation * transform_.rotation;
	updateTransform();
}

void TetoModel::scale(glm::vec3 scale)
{
	transform_.scale += scale;
	updateTransform();
}

void TetoModel::show()
{
	visible_ = true;
}

void TetoModel::hide()
{
	visible_ = false;
}

void TetoModel::setParent(TetoModel * parent)
{
	parent_ = parent;
}

std::pair<uint32_t, TetoInstance> TetoModel::pushMesh(std::string name)
{
	auto handle   = app_->pullResource(name);
	auto meshView = app_->getMesh(handle);

	if (!meshView) return {-1, {}};
	auto mesh = meshView.get();

	auto materialInstance = app_->generateMaterial(mesh->material_);

	transformList_.push_back(Transform {});
	materialList_ .push_back(materialInstance);

	auto instance = app_->instanciate(handle, transformList_.back().model(), materialList_.back());

	meshList_.push_back(instance);

	return {meshList_.size() - 1, instance};
}

Transform & TetoModel::getTransform(uint32_t index)
{
	return transformList_.at(index);
}

Material  & TetoModel::getMaterial(uint32_t index)
{
	return materialList_.at(index);
}

std::optional<Quad> TetoModel::getTexture(std::string name)
{
	auto handle    = app_->pullResource(name);
	auto imageView = app_->getImage(handle);

	if (!imageView) return std::nullopt;

	return app_->registerTexture(handle);
}

void TetoModel::updateMaterial()
{
	dirtyMaterial_ = true;
}

void TetoModel::updateTransform()
{
	dirtyTransform_ = true;
}
