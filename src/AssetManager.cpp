#include <stb/stb_image.h>

#include <AssetManager.hpp>
#include <Material/MaterialInstance.hpp>
#include <Core/Logger.hpp>
#include <Internal/Hash.hpp>
#include <Internal/ReadFile.hpp>
#include <Resources/Texture.hpp>
#include <Resources/ShaderSource.hpp>
#include <Geometry.hpp>


using namespace TLOT;

#define GET_SINGLETON AssetManager * instance = & GetInstance ();

AssetManager::AssetManager ()
{

}

ResourceView<Texture> AssetManager::GetTexture(ResourceHandle handle)
{
	GET_SINGLETON
	return instance->m_textures.Get (handle);
}

ResourceView<ShaderSource> AssetManager::GetShaderSource(ResourceHandle handle)
{
	GET_SINGLETON
	return instance->m_shaderSources.Get (handle);
}

ResourceView<Geometry> AssetManager::GetGeometry(ResourceHandle handle)
{
	GET_SINGLETON
	return instance->m_geometries.Get (handle);
}

ResourceView<MaterialTemplateSTD430> AssetManager::GetMaterialTemplate(ResourceHandle handle)
{
	GET_SINGLETON
	return instance->m_materials.Get(handle);
}

ResourceHandle AssetManager::CreateGeometry(
	std::vector<float> vertices,
	std::vector<uint32_t> indices,
	std::shared_ptr<VertexTemplate> vertexTemplate
)
{
	GET_SINGLETON

	return instance->m_geometries.Create (vertices, indices, vertexTemplate);
}

ResourceHandle AssetManager::CreateMaterialTemplate (UniformBlueprint && uniforms)
{
	GET_SINGLETON
	return instance->m_materials.Create(uniforms);
}

MaterialInstance AssetManager::CreateMaterial(ResourceHandle materialTemplate)
{
	GET_SINGLETON
	auto material = instance->m_materials.Get(materialTemplate);

	if (material.HasValue())
	{
		return MaterialInstance {materialTemplate, material->Size()};
	}

	return MaterialInstance {materialTemplate, 0};
}

ResourceHandle AssetManager::LoadTexture(std::string path)
{
	GET_SINGLETON
	return instance->m_textures.Load (path);
}

ResourceHandle AssetManager::LoadShaderSource(std::string path)
{
	GET_SINGLETON
	return instance->m_shaderSources.Load (path);
}

void AssetManager::Cache(std::string key, ResourceHandle handle)
{
	GET_SINGLETON

	instance->m_keyCache[key] = handle;
	instance->m_keyCacheReversed[handle] = key;
}
ResourceHandle AssetManager::Cache(std::string key)
{
	GET_SINGLETON

	if (instance->m_keyCache.find (key) == instance->m_keyCache.end ())
	{
		return InvalidResource;
	}

	return instance->m_keyCache[key];
}

template <>
bool Resource<Texture>::LoadFromDisk(std::string path)
{
	m_path = path;

	int nb_channels;
	int width;
	int height;

	m_resource.data = stbi_load (path.c_str (), &width, &height, &nb_channels, 4);

	if (!m_resource.data)
	{
		Logger::log (LogLevel::Error, "Can't load texture : {} ; {}", path, stbi_failure_reason ());
		m_resource.data = (unsigned char*)malloc (4 * sizeof (char));
		m_resource.data[0] = 255;
		m_resource.data[1] = 255;
		m_resource.data[2] = 255;
		m_resource.data[3] = 255;
		m_resource.width = 1;
		m_resource.height = 1;
		return false;
	}

	m_resource.width = width;
	m_resource.height = height;

	return true;
}

template <>
bool Resource<ShaderSource>::LoadFromDisk(std::string path)
{
	m_path = path;

	auto bufferOpt = read_file (path);

	if (!bufferOpt.has_value ())
	{
		Logger::log (LogLevel::Error, "Can't load shaderSource : {} ; {}", path, stbi_failure_reason ());
		return false;
	}

	m_resource.source = bufferOpt.value ();

	return true;
}

template <>
template <>
bool TLOT::Resource<TLOT::Geometry>
::Create<
	std::vector<float> &,
	std::vector<uint32_t> &,
	std::shared_ptr<TLOT::VertexTemplate> &
>
(
	std::vector<float> & vertices,
	std::vector<uint32_t> & indices,
	std::shared_ptr<TLOT::VertexTemplate> & vertexTemplate
)
{
	m_resource.vertices = vertices;
	m_resource.indices  = indices;
	m_resource.vertexTemplate = vertexTemplate;

	return true;
}

template<>
template<>
bool
TLOT::Resource<TLOT::MaterialTemplateSTD430>
::Create<TLOT::AssetManager::UniformBlueprint &>(TLOT::AssetManager::UniformBlueprint & uniforms)
{
	for (auto params : uniforms)
	{
		m_resource.AddUniform (params.first, params.second);
	}

	return true;
}
