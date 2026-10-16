
#include <TetoEngine/Application/Application.hpp>

#include <TetoEngine/Private/AssetManager/AssetManager.hpp>
#include <TetoEngine/Private/Renderer/Renderer.hpp>
#include <TetoEngine/Private/Renderer/MaterialTemplateSTD430.hpp>
#include <TetoEngine/Private/Window/Window.hpp>

#include <TetoEngine/Utils/Logger.hpp>
#include <TetoEngine/Utils/ReadFile.hpp>

#include "stb/stb_image.h"

using namespace TetoEngine;


struct Application::Impl
{
	Window window;
	std::map<std::string, ID64> resourceCache;
	AssetManager assetManager;
	Renderer renderer;

	std::string title = "Application";
	uint32_t width    = 800;
	uint32_t height   = 800;

	void initWindow()
	{
		window.init(width, height, title, false);
	}
};

void Application::run()
{
	impl_->initWindow();
	init();

	while (1)
	{
		glfwPollEvents();
		update();

		if (impl_->window.shouldClose())
			break;
		
		glClear(GL_COLOR_BUFFER_BIT);

		render();
		impl_->renderer.render();

		
		impl_->window.swapBuffers();
	}
	clean();
	//impl_->renderer.clean();
}

/* Assets */
ID64 Application::loadMaterial(MaterialLayout layout)
{
	return impl_->assetManager.loadMaterial(layout);
}

ID64 Application::loadShader(std::string path)
{
	return impl_->assetManager.loadShader(path);
}

ID64 Application::loadGeometry(std::vector<float> vertices, std::vector<uint32_t> indices)
{
	return impl_->assetManager.loadGeometry(vertices, indices);
}

ID64 Application::loadImage(std::string path)
{
	return impl_->assetManager.loadImage(path);
}

ID64 Application::loadFont(std::string path)
{
	return impl_->assetManager.loadFont(path);
}

ID64 Application::loadMesh(ID64 geometry, ID64 material)
{
	return impl_->assetManager.loadMesh(geometry, material);
}

ID64 Application::loadMaterial(std::string key, MaterialLayout layout)
{
	auto handle = impl_->assetManager.loadMaterial(layout);
	cacheResource(key, handle);
	return handle;
}

ID64 Application::loadShader(std::string key, std::string path)
{
	auto handle = impl_->assetManager.loadShader(path);
	cacheResource(key, handle);
	return handle;
}

ID64 Application::loadGeometry(std::string key, std::vector<float> vertices, std::vector<uint32_t> indices)
{
	auto handle = impl_->assetManager.loadGeometry(vertices, indices);
	cacheResource(key, handle);
	return handle;
}

ID64 Application::loadImage(std::string key, std::string path)
{
	auto handle = impl_->assetManager.loadImage(path);
	cacheResource(key, handle);
	return handle;
}

ID64 Application::loadFont(std::string key, std::string path)
{
	auto handle = impl_->assetManager.loadFont(path);
	cacheResource(key, handle);
	return handle;
}

ID64 Application::loadMesh(std::string key, ID64 geometry, ID64 material)
{
	auto handle = impl_->assetManager.loadMesh(geometry, material);
	cacheResource(key, handle);
	return handle;
}

ResourceView<MaterialLayout> Application::getMaterialLayout(ID64 handle)
{
	return impl_->assetManager.getMaterialLayout(handle);
}

ResourceView<ShaderSource> Application::getShaderSource(ID64 handle)
{
	return impl_->assetManager.getShaderSource(handle);
}

ResourceView<Geometry> Application::getGeometry(ID64 handle)
{
	return impl_->assetManager.getGeometry(handle);
}

ResourceView<Image> Application::getImage(ID64 handle)
{
	return impl_->assetManager.getImage(handle);
}

ResourceView<Font> Application::getFont(ID64 handle)
{
	return impl_->assetManager.getFont(handle);
}

ResourceView<Mesh> Application::getMesh(ID64 handle)
{
	return impl_->assetManager.getMesh(handle);
}

void Application::unloadGeometry(ID64 resource)
{
	impl_->assetManager.unloadGeometry(resource);
}

void Application::unloadImage(ID64 resource)
{
	impl_->assetManager.unloadImage(resource);
}

void Application::unloadShader(ID64 resource)
{
	impl_->assetManager.unloadShader(resource);
}

void Application::unloadMaterial(ID64 resource)
{
	impl_->assetManager.unloadMaterial(resource);
}

void Application::unloadMesh(ID64 resource)
{
	impl_->assetManager.unloadMesh(resource);
}

void Application::cacheResource(std::string name, ID64 resource)
{
	impl_->resourceCache.emplace(name, resource);
}

ID64 Application::pullResource(std::string name)
{
	if (impl_->resourceCache.contains(name))
		return impl_->resourceCache.at(name);

	return InvalidID;
}

/* Misc */

double Application::deltaTime()
{

}

/* Rendering */

void Application::registerTechnique(Technique technique)
{
	impl_->renderer.registerTechnique(technique);
}

TRID Application::registerTexture(ID64 image)
{
	impl_->renderer.registerTexture(image);
}

TRID Application::createFont(ID64 font)
{
	impl_->renderer.createFont(font);
}

TRID Application::createProgram(ID64 vertexHandle, ID64 fragmentHandle)
{
	impl_->renderer.createProgram(vertexHandle, fragmentHandle);
}

Material Application::generateMaterial(ID64 handle)
{
	impl_->renderer.generateMaterial(handle);
}

TIRID Application::instanciate(ID64 meshHandle, glm::mat4 model, Material material)
{
	impl_->renderer.instanciate(meshHandle, model, material);
}

void Application::updateInstance(TIRID instance, glm::mat4 model, Material material)
{
	impl_->renderer.updateInstance(instance, model, material);
}

void Application::updateInstance(TIRID instance, glm::mat4 model)
{
	impl_->renderer.updateInstance(instance, model);
}

void Application::updateInstance(TIRID instance, Material material)
{
	impl_->renderer.updateInstance(instance, material);
}

void Application::drawInstance(TIRID instance)
{
	impl_->renderer.drawInstance(instance);
}

void Application::killInstance(TIRID instance)
{
	impl_->renderer.killInstance(instance);
}

void Application::render()
{
	impl_->renderer.render();
}

void Application::setTitle(std::string title)
{
	impl_->title = title;
	reloadWindow();
}

void Application::setWidth(uint32_t width)
{
	impl_->width = width;
	reloadWindow();
}

void Application::setHeight(uint32_t height)
{
	impl_->height = height;
	reloadWindow();
}

void Application::setSize(uint32_t width, uint32_t height)
{
	impl_->width = width;
	impl_->height = height;
	reloadWindow();
}

void Application::reloadWindow()
{
	impl_->window.resize  (impl_->width, impl_->height);
	impl_->window.setTitle(impl_->title);
}
