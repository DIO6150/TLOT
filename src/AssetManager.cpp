#include <modules/AssetManager.hpp>

#include <core/Logger.hpp>

#include <core/Utils.hpp>

using namespace TLOT;

AssetManager::AssetManager ()
{
	{
		std::vector<Vertex> const quadVertices = {
			{-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f},
			{ 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f},
			{ 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f},
			
			{-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f},
			{ 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f},
			{-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f}  
		};
		
		std::vector<uint32_t> const quadIndices = {0, 1, 2, 3, 4, 5};

		m_quadMeshID = m_meshes.PushResource ("__TLOT_QUAD_MESH", quadVertices, quadIndices, Material {}, m_nextMeshID++);
	}

	{
		unsigned char *textureData = new unsigned char[4];
		textureData[0] = 255;
		textureData[1] = 255;
		textureData[2] = 255;
		textureData[3] = 255;
		m_textures.CreateDefault (textureData, 1, 1);
	}

	{
		unsigned char * textureData = new unsigned char[16];
		textureData[0]  = 0;
		textureData[1]  = 0;
		textureData[2]  = 0;
		textureData[3]  = 255;

		textureData[4]  = 255;
		textureData[5]  = 0;
		textureData[6]  = 0;
		textureData[7]  = 255;

		textureData[8]  = 0;
		textureData[9]  = 0;
		textureData[10] = 0;
		textureData[11] = 255;

		textureData[12] = 255;
		textureData[13] = 0;
		textureData[14] = 0;
		textureData[15] = 255;

		m_missingTextureID = m_textures.PushResource ("__TLOT_MISSING_TEXTURE", textureData, 1, 1);
	}
}

ResourceHandle AssetManager::LoadTexture (std::string key, fs::path path)
{
	if (m_textures.KeyExists (key))
	{
		return m_shaders.GetHandleFromKey (key);
	}
	
	unsigned char * data;
	size_t width, height;

	if (!load_texture (path, data, width, height))
	{
		Logger::log (LogLevel::Error, "Could not load or create texture key={} path={}.", key, path.string ());
		return m_missingTextureID;
	}
	
	return m_textures.PushResource (key, data, width, height);
}

ResourceHandle AssetManager::LoadMesh (std::string key, fs::path path)
{
	Logger::log (LogLevel::Info, "Trying to load Mesh path={} key={}", path.string (), key);
	Logger::log (LogLevel::Warning, "Loading meshes from disk is not supported at the moment.");
	return InvalidResource;
}

ResourceHandle AssetManager::LoadShader (std::string key, fs::path vertexPath, fs::path fragmentPath)
{
	// TODO: We should load only the text and compile then link shaders only in the renderer
	// since it's technichally an opengl object

	if (m_shaders.KeyExists (key))
	{
		return m_shaders.GetHandleFromKey (key);
	}

	ResourceHandle handle = m_shaders.PushResource (key, vertexPath.string (), fragmentPath.string ());

	return handle;
}

// TODO: should only returns a copy of the resoure
// can't do it now because I need to rework some other classes that rely on having a copy instead
// on top of my head it is mostly shaders and mesh

Texture & AssetManager::GetTexture (ResourceHandle handle)
{
	return m_textures.GetResource (handle);
}

Mesh & AssetManager::GetMesh (ResourceHandle handle)
{
	return m_meshes.GetResource (handle);
}

Shader & AssetManager::GetShader (ResourceHandle handle)
{
	return m_shaders.GetResource (handle);
}

ResourceHandle AssetManager::GetTextureID (std::string key)
{
	return m_textures.GetHandleFromKey (key);
}

ResourceHandle AssetManager::GetQuadMeshID ()
{
	return m_quadMeshID;
}
