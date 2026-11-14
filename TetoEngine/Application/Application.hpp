#pragma once

#include <TetoEngine/Application/AssetManagerInterface.hpp>

#include <TetoEngine/Renderer/TRID.hpp>

#include <TetoEngine/Common/Transform.hpp>
#include <TetoEngine/Common/Material.hpp>

#include <TetoEngine/Renderer/Technique.hpp>

#include <TetoEngine/Common/Viewport.hpp>


namespace TetoEngine
{
	class Application: public AssetManagerInterface
	{
	public:
		Application();

		/* Main methods */
		void run();

		/* Assets */
		ID64 loadMaterial(MaterialLayout layout) override;
		ID64 loadShader(std::string path) override;
		ID64 loadGeometry(std::vector<float> vertices, std::vector<uint32_t> indices) override;
		ID64 loadImage(std::string path) override;
		ID64 loadFont(std::string path) override;
		ID64 loadMesh(ID64 geometry, ID64 material) override;

		ID64 loadMaterial(std::string key, MaterialLayout layout) override;
		ID64 loadShader  (std::string key, std::string path) override;
		ID64 loadGeometry(std::string key, std::vector<float> vertices, std::vector<uint32_t> indices) override;
		ID64 loadImage   (std::string key, std::string path) override;
		ID64 loadFont    (std::string key, std::string path) override;
		ID64 loadMesh    (std::string key, ID64 geometry, ID64 material) override;

		ResourceView<MaterialLayout> getMaterialLayout(ID64 handle) override;
		ResourceView<ShaderSource> getShaderSource(ID64 handle) override;
		ResourceView<Geometry> getGeometry(ID64 handle) override;
		ResourceView<Image> getImage(ID64 handle) override;
		ResourceView<Font> getFont(ID64 handle) override;
		ResourceView<Mesh> getMesh(ID64 handle) override;

		void unloadGeometry(ID64 resource) override;
		void unloadImage(ID64 resource) override;
		void unloadShader(ID64 resource) override;
		void unloadMaterial(ID64 resource) override;
		void unloadMesh(ID64 resource) override;

		void cacheResource(std::string name, ID64 resource) override;
		ID64 pullResource(std::string name) override;
	
		/* Misc */
		double deltaTime();
		Viewport viewport();
		
		/* Rendering */
		void reloadGPUResources();

		void registerTechnique(Technique technique);
		TRID registerTexture(ID64 handle);
		
		TRID createFont(ID64 handle);
		TRID generateProgram(ID64 vertexHandle, ID64 fragmentHandle);

		Material generateMaterial(ID64 handle);


		TetoInstance instanciate(ID64 meshHandle, glm::mat4 model, Material material);
		void updateInstance(TetoInstance instance, glm::mat4 model, Material material);
		void updateInstance(TetoInstance instance, glm::mat4 model);
		void updateInstance(TetoInstance instance, Material material);
		void drawInstance(TetoInstance object);
		void killInstance(TetoInstance object);

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
		std::indirect<Impl> impl_;
	};
}