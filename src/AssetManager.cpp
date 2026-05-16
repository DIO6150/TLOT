#include <Core/Logger.hpp>

#include <AssetManager.hpp>

#include <Internal/Hash.hpp>
#include <Internal/ReadFile.hpp>

#include <Resources/Texture.hpp>
#include <Resources/ShaderSource.hpp>

#include <MeshData/Vertex.hpp>

#include <stb/stb_image.h>

using namespace TLOT;

#define GET_SINGLETON AssetManager * instance = & GetInstance ();

AssetManager::AssetManager ()
{
	{
		Vertices_t const quadVertices = {
			{-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f},
			{ 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f},
			{ 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f},
			
			{-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f},
			{ 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f},
			{-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f}  
		};
		
		Indices_t const quadIndices = {0, 1, 2, 3, 4, 5};

		m_quadMesh = Mesh {quadVertices, quadIndices, Material {}, 0};
	}
//
	//{
	//	unsigned char *textureData = new unsigned char[4];
	//	textureData[0] = 255;
	//	textureData[1] = 255;
	//	textureData[2] = 255;
	//	textureData[3] = 255;
	//	//m_textures.CreateDefault (textureData, 1, 1);
	//}
//
	//{
	//	unsigned char * textureData = new unsigned char[16];
	//	textureData[0]  = 0;
	//	textureData[1]  = 0;
	//	textureData[2]  = 0;
	//	textureData[3]  = 255;
//
	//	textureData[4]  = 255;
	//	textureData[5]  = 0;
	//	textureData[6]  = 0;
	//	textureData[7]  = 255;
//
	//	textureData[8]  = 255;
	//	textureData[9]  = 0;
	//	textureData[10] = 0;
	//	textureData[11] = 255;
//
	//	textureData[12] = 0;
	//	textureData[13] = 0;
	//	textureData[14] = 0;
	//	textureData[15] = 255;
//
	//	//m_missingTextureID = m_textures.PushResource ("__TLOT_MISSING_TEXTURE", textureData, 2, 2);
	//}
}

Mesh const & AssetManager::GetQuadMesh ()
{
	GET_SINGLETON
	return instance->m_quadMesh;
}

Texture const & AssetManager::GetTexture (ResourceHandle handle)
{
	GET_SINGLETON
	return instance->m_textures.Get (handle);
}

ShaderSource const & AssetManager::GetShaderSource (ResourceHandle handle)
{
	GET_SINGLETON
	return instance->m_shaderSources.Get (handle);
}

ResourceHandle AssetManager::CreateShaderCollection (ResourceHandle vertex, ResourceHandle fragment)
{
	GET_SINGLETON
	ResourceHandle handle = GenerateHandle ();

	instance->m_shaderCollections.emplace (handle, ProtoShader {vertex, fragment, handle});

	return handle;
}

ProtoShader AssetManager::GetShaderCollection (ID_64 hash)
{
	GET_SINGLETON
	return instance->m_shaderCollections[hash];
}

ResourceHandle AssetManager::LoadTexture (std::string path)
{
	GET_SINGLETON
	return instance->m_textures.Load (path);
}

ResourceHandle AssetManager::LoadShaderSource (std::string path)
{
	GET_SINGLETON
	return instance->m_shaderSources.Load (path);
}

void AssetManager::Cache (std::string key, ResourceHandle handle)
{
	GET_SINGLETON

	instance->m_keyCache[key] = handle;
	instance->m_keyCacheReversed[handle] = key;
}
ResourceHandle AssetManager::Cache (std::string key)
{
	GET_SINGLETON

	if (instance->m_keyCache.find (key) == instance->m_keyCache.end ())
	{
		return InvalidResource;
	}

	return instance->m_keyCache[key];
}

template <>
bool Resource<Texture>::LoadFromDisk (std::string path)
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
bool Resource<ShaderSource>::LoadFromDisk (std::string path)
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
