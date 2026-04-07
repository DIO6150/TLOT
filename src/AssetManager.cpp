#include <modules/AssetManager.hpp>

#include <core/Logger.hpp>

#include <core/Utils.hpp>

using namespace TLOT;

AssetManager::AssetManager ()
{
	// setup default quad
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

		m_quadMeshID = m_meshes.PushResource ("__TLOT_QUAD", quadVertices, quadIndices, Material {}, m_nextMeshID++);
	}
}

ResourceHandle AssetManager::LoadTexture (std::string key, fs::path path)
{
	

	if (m_textures.KeyExists (key))
	{
		return m_textures.GetValue (key);
	}
	
	unsigned char * data;
	size_t width, height;
	if (load_texture (path, data, width, height))
	{
		ResourceHandle handle = m_textures.PushResource (key, data, width, height);

		return handle;
	}

	Logger::log (LogLevel::Error, "Could not create texture : {}.", path.string ());
	return InvalidResource;
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
		return m_shaders.GetValue (key);
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

ResourceHandle AssetManager::GetQuadMeshID ()
{
	return m_quadMeshID;
}
