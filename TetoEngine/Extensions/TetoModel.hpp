#pragma once

#include <glm/glm.hpp>

#include <TetoEngine/Common/Transform.hpp>
#include <TetoEngine/Common/Material.hpp>

#include <TetoEngine/Renderer/TRID.hpp>

namespace TetoEngine
{
	class Application;

	class TetoModel
	{
	public:
		void init(Application * app);
		void destroy() {}
		void update() {}
		void draw();

		glm::mat4 matrix  () const;
		glm::vec3 position() const;
		glm::quat rotation() const;
		glm::vec3 scale   () const;

		bool isVisible() const;

		Transform transform() const;

		void setPosition(glm::vec3 position);
		void setRotation(glm::quat rotation);
		void setRotation(glm::vec3 eulerAngle);
		void setScale   (glm::vec3 scale);

		void translate(glm::vec3 translation);
		void rotate   (glm::quat rotation);
		void scale    (glm::vec3 scale);

		void show();
		void hide();

		void setParent(TetoModel * parent); // useful for computing transformation

	protected:
		std::pair<uint32_t, TIRID> pushMesh(std::string name);
		
		Transform & getTransform(uint32_t index);
		Material  & getMaterial (uint32_t index);
		
		glm::vec4 getTexture(std::string name); // returns the handle of the texture in the renderer not in the assetManager
		
		void updateMaterial();
		void updateTransform();

	protected:
		Application * app_ = nullptr;

	private:
		bool visible_ = true;

		Transform transform_;

		std::vector<TIRID>     meshList_;
		std::vector<Transform> transformList_;
		std::vector<Material>  materialList_;

		bool dirtyTransform_ = true;
		bool dirtyMaterial_  = true;

		TetoModel * parent_ = nullptr;
	};
}