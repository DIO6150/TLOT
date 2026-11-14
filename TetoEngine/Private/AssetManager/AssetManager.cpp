
#include <TetoEngine/Private/AssetManager/AssetManager.hpp>

#include <TetoEngine/Utils/Logger.hpp>
#include <TetoEngine/Utils/ReadFile.hpp>

#include <stb/stb_image.h>

using namespace TetoEngine;

ID64 AssetManager::loadMaterial(MaterialLayout layout)
{
	auto handle = materialCache_.create();

	if (handle == InvalidID)
	{
		Logger::log(LogLevel::Error, "Can't create resource for material");
		return InvalidID;
	}

	auto materialView = materialCache_.get(handle);

	if (!materialView)
	{
		Logger::log(LogLevel::Error, "Can't get view for material");
		return InvalidID;
	}

	auto material = materialView.get();

	material->shader = layout.shader;
	material->shaderProperties   = layout.shaderProperties;
	material->instanceProperties = layout.instanceProperties;

	return handle;
}

ID64 AssetManager::loadShader(std::string path)
{
	auto handle = shaderCache_.create();

	if (handle == InvalidID)
	{
		Logger::log(LogLevel::Error, "Can't create resource for shaderSource : {}", path);

		return InvalidID;
	}

	auto shaderView = shaderCache_.get(handle);

	if (!shaderView)
	{
		Logger::log(LogLevel::Error, "Can't get view for shaderSource : {}", path);

		return InvalidID;
	}

	auto shaderSource = shaderView.get();

	auto bufferOpt = read_file(path);
	if (!bufferOpt.has_value())
	{
		Logger::log(LogLevel::Error, "Can't load shaderSource : {}", path);
		return missingShader();
	}

	shaderSource->source_ = bufferOpt.value();

	return handle;
}

ID64 AssetManager::loadGeometry(std::vector<float> vertices, std::vector<uint32_t> indices)
{
	auto handle = geometryCache_.create();

	if (handle == InvalidID)
	{
		Logger::log(LogLevel::Error, "Can't create resource for geometry");
		return InvalidID;
	}

	auto geometryView = geometryCache_.get(handle);

	if (!geometryView)
	{
		Logger::log(LogLevel::Error, "Can't get view for geometry");
		return InvalidID;
	}

	auto geometry = geometryView.get();
	geometry->vertices_ = vertices;
	geometry->indices_  = indices;

	return handle;
}

ID64 AssetManager::loadImage(std::string path)
{
	auto handle = imageCache_.create();
	if (handle == InvalidID)
	{
		Logger::log(LogLevel::Error, "Couldn't create resource for texture : {}", path);
		return InvalidID;
	}

	auto textureView = imageCache_.get(handle);
	if (!textureView)
	{
		Logger::log(LogLevel::Error, "Couldn't get view for texture : {}", path);
		return InvalidID;
	}

	auto texture = textureView.get();

	int width, height, channels;
	unsigned char * data;

	data = stbi_load(path.c_str (), &width, &height, &channels, 4);

	if (!data)
	{
		Logger::log(LogLevel::Error, "Can't load texture : {} ({})", path, stbi_failure_reason());
		return missingTexture();
	}

	texture->width    = width;
	texture->height   = height;
	texture->channels = channels;

	texture->data.resize(width * height * channels);
	memcpy(texture->data.data(), data, texture->data.size());

	return handle;
}

ID64 AssetManager::loadFont(std::string path)
{
	auto handle = fontCache_.create();

	if (handle == InvalidID)
	{
		Logger::log(LogLevel::Error, "Can't create resource for font : {}", path);

		return InvalidID;
	}

	auto fontView = fontCache_.get(handle);

	if (!fontView)
	{
		Logger::log(LogLevel::Error, "Can't get view for font : {}", path);

		return InvalidID;
	}

	auto font = fontView.get();

	auto buffer = read_file(path);
	if (!buffer)
	{
		return InvalidID;
	}

	font->buffer_ = buffer.value();

	return handle;
}

ID64 AssetManager::loadMesh(ID64 geometry, ID64 material)
{
	auto handle = meshCache_.create();

	if (handle == InvalidID)
	{
		Logger::log(LogLevel::Error, "Can't create resource for mesh");
		return InvalidID;
	}

	auto meshView = meshCache_.get(handle);

	if (!meshView)
	{
		Logger::log(LogLevel::Error, "Can't get view for mesh");
		return InvalidID;
	}

	auto mesh = meshView.get();
	mesh->geometry_ = geometry;
	mesh->material_ = material;

	return handle;
}

ResourceView<MaterialLayout> AssetManager::getMaterialLayout(ID64 handle)
{
	return materialCache_.get(handle);
}

ResourceView<ShaderSource> AssetManager::getShaderSource(ID64 handle)
{
	return shaderCache_.get(handle);
}

ResourceView<Geometry> AssetManager::getGeometry(ID64 handle)
{
	return geometryCache_.get(handle);
}

ResourceView<Image> AssetManager::getImage(ID64 handle)
{
	return imageCache_.get(handle);
}

ResourceView<Font> AssetManager::getFont(ID64 handle)
{
	return fontCache_.get(handle);
}

ResourceView<Mesh> AssetManager::getMesh(ID64 handle)
{
	return meshCache_.get(handle);
}


void AssetManager::unloadGeometry(ID64 resource)
{
	geometryCache_.destroy(resource);
}

void AssetManager::unloadImage(ID64 resource)
{
	imageCache_.destroy(resource);
}

void AssetManager::unloadShader(ID64 resource)
{
	shaderCache_.destroy(resource);
}

void AssetManager::unloadMaterial(ID64 resource)
{
	materialCache_.destroy(resource);
}

void AssetManager::unloadMesh(ID64 resource)
{
	meshCache_.destroy(resource);
}