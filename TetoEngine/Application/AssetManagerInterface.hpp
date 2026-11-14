#pragma once

#include <TetoEngine/Common/ID.hpp>

#include <TetoEngine/Resource/MaterialLayout.hpp>
#include <TetoEngine/Resource/Image.hpp>
#include <TetoEngine/Resource/ShaderSource.hpp>
#include <TetoEngine/Resource/Font.hpp>
#include <TetoEngine/Resource/Geometry.hpp>
#include <TetoEngine/Resource/ResourceView.hpp>
#include <TetoEngine/Resource/Mesh.hpp>

namespace TetoEngine
{
	struct AssetManagerInterface
	{
		virtual ID64 loadMaterial(MaterialLayout layout) = 0;
		virtual ID64 loadShader(std::string path) = 0;
		virtual ID64 loadGeometry(std::vector<float> vertices, std::vector<uint32_t> indices) = 0;
		virtual ID64 loadImage(std::string path) = 0;
		virtual ID64 loadFont(std::string path) = 0;
		virtual ID64 loadMesh(ID64 geometry, ID64 material) = 0;

		virtual ID64 loadMaterial(std::string key, MaterialLayout layout) = 0;
		virtual ID64 loadShader  (std::string key, std::string path) = 0;
		virtual ID64 loadGeometry(std::string key, std::vector<float> vertices, std::vector<uint32_t> indices) = 0;
		virtual ID64 loadImage   (std::string key, std::string path) = 0;
		virtual ID64 loadFont    (std::string key, std::string path) = 0;
		virtual ID64 loadMesh    (std::string key, ID64 geometry, ID64 material) = 0;

		virtual ResourceView<MaterialLayout> getMaterialLayout(ID64 handle) = 0;
		virtual ResourceView<ShaderSource> getShaderSource(ID64 handle) = 0;
		virtual ResourceView<Geometry> getGeometry(ID64 handle) = 0;
		virtual ResourceView<Image> getImage(ID64 handle) = 0;
		virtual ResourceView<Font> getFont(ID64 handle) = 0;
		virtual ResourceView<Mesh> getMesh(ID64 handle) = 0;

		virtual void unloadGeometry(ID64 resource) = 0;
		virtual void unloadImage(ID64 resource) = 0;
		virtual void unloadShader(ID64 resource) = 0;
		virtual void unloadMaterial(ID64 resource) = 0;
		virtual void unloadMesh(ID64 resource) = 0;

		virtual void cacheResource(std::string name, ID64 resource) = 0;
		virtual ID64 pullResource(std::string name) = 0;
	};
	
}