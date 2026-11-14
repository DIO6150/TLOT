
#include <TetoEngine/Private/GPUResources/GPUResourceManager.hpp>
#include <TetoEngine/Application/Application.hpp>

#include <TetoEngine/Resource/Geometry.hpp>

using namespace TetoEngine;

GPUResourceManager::GPUResourceManager(Application * app)
	: app_ {app}
{

}

void GPUResourceManager::reserveGeometry(ID64 handle)
{
	auto geometryView = app_->getGeometry(handle);

	if (!geometryView)
		return;

	uint32_t index = 0;

	if (vertices_.buffer.empty())
	{
		OpenGLBuffer     buffer {GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, defaultVBOSize_};
		GPUResourceTable table  {defaultVBOSize_};

		vertices_.buffer.emplace_back(std::move(buffer));
		vertices_.table .emplace_back(std::move(table));
	}
	else
	{
		size_t vertexSize = geometryView->vertices_.size();

		for (auto index = 0; index < vertices_.buffer.size(); ++index)
		{
			auto & table = vertices_.table.at(index);

			if (geometryView->vertices_.size() > table.remaining())
				continue;

			auto & buffer = vertices_.buffer.at(index);
			auto offset = table.add(handle, vertexSize);

			if (!offset)
				continue;
		}
	}

	// indices here
}

void GPUResourceManager::uploadGeometry(ID64 handle)
{
	auto geometryView = app_->getGeometry(handle);

	if (!geometryView)
		return;

	uint32_t index = 0;

	if (vertices_.buffer.empty())
	{
		OpenGLBuffer     buffer {GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, defaultVBOSize_};
		GPUResourceTable table  {defaultVBOSize_};

		vertices_.buffer.emplace_back(std::move(buffer));
		vertices_.table .emplace_back(std::move(table));
	}

	size_t vertexSize = geometryView->vertices_.size();

	for (auto index = 0; index < vertices_.buffer.size(); ++index)
	{
		auto & table = vertices_.table.at(index);

		if (geometryView->vertices_.size() > table.remaining())
			continue;

		auto & buffer = vertices_.buffer.at(index);
		auto offset = table.add(handle, vertexSize);

		if (!offset)
			continue;

		buffer.upload(geometryView->vertices_, offset.value());
		vertices_.location.emplace(handle, index);
		break;
	}
}

void GPUResourceManager::unloadGeometry(ID64 handle)
{

}

std::pair<uint32_t, uint32_t> GPUResourceManager::geometryByteOffset(ID64 handle)
{

}

std::pair<uint32_t, uint32_t> GPUResourceManager::geometryBuffer(ID64 handle)
{

}

bool GPUResourceManager::containsGeometry(ID64 handle)
{

}



std::optional<size_t> GPUResourceManager::findVertexDrawerIndex(ID64 handle)
{
	auto geometryView = app_->getGeometry(handle);

	if (!geometryView)
		return std::nullopt;

	if (vertices_.buffer.empty())
	{
		OpenGLBuffer     buffer {GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, defaultVBOSize_};
		GPUResourceTable table  {defaultVBOSize_};

		vertices_.buffer.emplace_back(std::move(buffer));
		vertices_.table .emplace_back(std::move(table));

		return 0;
	}

	size_t vertexSize = geometryView->vertices_.size();

	for (auto index = 0; index < vertices_.buffer.size(); ++index)
	{
		auto & table = vertices_.table.at(index);

		if (geometryView->vertices_.size() > table.remaining())
			continue;

		auto & buffer = vertices_.buffer.at(index);
		auto offset = table.add(handle, vertexSize);

		if (!offset)
			continue;

		return index;
	}
}
