#pragma once

#include <TetoEngine/Common/ID.hpp>

#include <TetoEngine/Resource/MaterialLayout.hpp>
#include <TetoEngine/Resource/Image.hpp>
#include <TetoEngine/Resource/ShaderSource.hpp>
#include <TetoEngine/Resource/Font.hpp>
#include <TetoEngine/Resource/Geometry.hpp>
#include <TetoEngine/Resource/ResourceView.hpp>
#include <TetoEngine/Resource/Mesh.hpp>

#include <TetoEngine/Renderer/TRID.hpp>

#include <TetoEngine/Common/Transform.hpp>
#include <TetoEngine/Common/Material.hpp>

#include <TetoEngine/Renderer/Technique.hpp>

#include <TetoEngine/Common/Viewport.hpp>


namespace TetoEngine
{
	class Application
	{
	public:
		/* Main methods */
		void run();

		/* Assets */
		ID64 loadMaterial(MaterialLayout layout);
		ID64 loadShader(std::string path);
		ID64 loadGeometry(std::vector<float> vertices, std::vector<uint32_t> indices);
		ID64 loadImage(std::string path);
		ID64 loadFont(std::string path);
		ID64 loadMesh(ID64 geometry, ID64 material);

		ID64 loadMaterial(std::string key, MaterialLayout layout);
		ID64 loadShader  (std::string key, std::string path);
		ID64 loadGeometry(std::string key, std::vector<float> vertices, std::vector<uint32_t> indices);
		ID64 loadImage   (std::string key, std::string path);
		ID64 loadFont    (std::string key, std::string path);
		ID64 loadMesh    (std::string key, ID64 geometry, ID64 material);

		ResourceView<MaterialLayout> getMaterialLayout(ID64 handle);
		ResourceView<ShaderSource> getShaderSource  (ID64 handle);
		ResourceView<Geometry> getGeometry(ID64 handle);
		ResourceView<Image> getImage(ID64 handle);
		ResourceView<Font> getFont(ID64 handle);
		ResourceView<Mesh> getMesh(ID64 handle);

		void unloadGeometry(ID64 resource);
		void unloadImage(ID64 resource);
		void unloadShader(ID64 resource);
		void unloadMaterial(ID64 resource);
		void unloadMesh(ID64 resource);

		void cacheResource(std::string name, ID64 resource);
		ID64 pullResource(std::string name);
	
		/* Misc */
		double deltaTime();
		Viewport viewport();
		
		/* Rendering */
		void registerTechnique(Technique technique);
		TRID registerTexture(ID64 handle);
		
		TRID createFont(ID64 handle);
		TRID createProgram(ID64 vertexHandle, ID64 fragmentHandle);

		Material generateMaterial(ID64 handle);


		TIRID instanciate(ID64 meshHandle, glm::mat4 model, Material material);
		void updateInstance(TIRID instance, glm::mat4 model, Material material);
		void updateInstance(TIRID instance, glm::mat4 model);
		void updateInstance(TIRID instance, Material material);
		void drawInstance(TIRID object);
		void killInstance(TIRID object);

	protected:
		void setTitle(std::string name);
		void setWidth(uint32_t width);
		void setHeight(uint32_t height);
		void setSize(uint32_t width, uint32_t height);
		void reloadWindow();

	private:
		virtual void init  () {}
		virtual void clean () {}
		virtual void update() {}
		virtual void render() {}

	private:
		struct Impl;
		std::unique_ptr<Impl> impl_;
	};
}