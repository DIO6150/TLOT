#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>

#include <Internal/ResourceCache.hpp>

#include <Resources/Texture.hpp>
#include <Resources/ShaderSource.hpp>

#include <Geometry.hpp>
#include <Material/MaterialTemplate.hpp>

namespace TLOT
{
	class MaterialInstance;

	class AssetManager
	{
	public:
		using UniformBlueprint = std::vector<std::pair<std::string, UniformType>>;

		static ResourceView<Texture> GetTexture(ResourceHandle handle);
		static ResourceView<ShaderSource> GetShaderSource(ResourceHandle handle);
		static ResourceView<Geometry> GetGeometry(ResourceHandle handle);
		static ResourceView<MaterialTemplateSTD430> GetMaterialTemplate(ResourceHandle handle);

		static ResourceHandle LoadTexture(std::string path);
		static ResourceHandle LoadShaderSource(std::string path);

		static ResourceHandle CreateGeometry(std::vector<float> vertices, std::vector<uint32_t> indices, std::shared_ptr<VertexTemplate> vertexTemplate);
		static ResourceHandle CreateMaterialTemplate(UniformBlueprint && uniforms);

		static MaterialInstance CreateMaterial(ResourceHandle materialTemplate);

		static void Cache(std::string key, ResourceHandle handle);
		static ResourceHandle Cache(std::string key);
		
	private:
		static AssetManager & GetInstance()
		{
			static AssetManager instance {};
			return instance;
		}

		ResourceCache<Texture> m_textures;
		ResourceCache<ShaderSource> m_shaderSources;
		ResourceCache<Geometry> m_geometries;
		ResourceCache<MaterialTemplateSTD430> m_materials;

		std::map<std::string, ResourceHandle> m_keyCache;
		std::map<ResourceHandle, std::string> m_keyCacheReversed;

		AssetManager();
		AssetManager(AssetManager && AssetManager) = delete;
		AssetManager(AssetManager const & AssetManager) = delete;
		AssetManager(AssetManager & AssetManager) = delete;
		AssetManager & operator=(AssetManager && AssetManager) = delete;
		AssetManager & operator=(AssetManager & AssetManager) = delete;
		AssetManager & operator=(AssetManager const & AssetManager) = delete;

		friend class SceneInspector;
	};
}

template<> bool TLOT::Resource<TLOT::Texture>::LoadFromDisk (std::string path);
template<> bool TLOT::Resource<TLOT::ShaderSource>::LoadFromDisk (std::string path);

template<> template<> bool TLOT::Resource<TLOT::Geometry>
::Create<
	std::vector<float> &,
	std::vector<uint32_t> &,
	std::shared_ptr<TLOT::VertexTemplate> &
>
(
	std::vector<float> & vertices,
	std::vector<uint32_t> & indices,
	std::shared_ptr<TLOT::VertexTemplate> & vertexTemplate
);

template<> template<> bool TLOT::Resource<TLOT::MaterialTemplateSTD430>
::Create<TLOT::AssetManager::UniformBlueprint &>(TLOT::AssetManager::UniformBlueprint & uniforms);

