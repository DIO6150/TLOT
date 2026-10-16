#pragma once

#include <memory>

#include <TetoEngine/Resource/Font.hpp>
#include <TetoEngine/Resource/Geometry.hpp>
#include <TetoEngine/Resource/MaterialLayout.hpp>
#include <TetoEngine/Resource/ResourceView.hpp>
#include <TetoEngine/Resource/ShaderSource.hpp>
#include <TetoEngine/Resource/Image.hpp>
#include <TetoEngine/Resource/Mesh.hpp>

#include <TetoEngine/Private/AssetManager/ResourceArray.hpp>

namespace TetoEngine
{
	class AssetManager
	{
	public:
		ID64 loadMaterial(MaterialLayout layout);
		ID64 loadShader(std::string path);
		ID64 loadGeometry(std::vector<float> vertices, std::vector<uint32_t> indices);
		ID64 loadImage(std::string path);
		ID64 loadFont(std::string path);
		ID64 loadMesh(ID64 geometry, ID64 material);

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

	private:
		ResourceArray<Image  >        imageCache_;
		ResourceArray<ShaderSource>   shaderCache_;
		ResourceArray<Geometry>       geometryCache_;
		ResourceArray<Font>           fontCache_;
		ResourceArray<MaterialLayout> materialCache_;
		ResourceArray<Mesh>           meshCache_;
		
		ID64 missingTexture_ = InvalidID;
		ID64 missingShader_  = InvalidID;
		
		ID64 missingTexture();
		ID64 missingShader();
	};
}
