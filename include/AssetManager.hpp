#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>

#include <Internal/ResourceCache.hpp>

#include <Resources/Texture.hpp>
#include <Resources/ShaderSource.hpp>

#include <MeshData/Mesh.hpp>

namespace TLOT
{
	struct ProtoShader
	{
		ResourceHandle vertex;
		ResourceHandle fragment;

		ResourceHandle handle;
	};

	class AssetManager
	{
	public:
		static Mesh const & GetQuadMesh ();

		static Texture const & GetTexture (ResourceHandle handle);
		static ShaderSource const & GetShaderSource (ResourceHandle handle);

		static ResourceHandle LoadTexture (std::string path);
		static ResourceHandle LoadShaderSource (std::string path);

		static ResourceHandle CreateShaderCollection (ResourceHandle vertex, ResourceHandle fragment);
		static ProtoShader GetShaderCollection (ResourceHandle hash);

		static void Cache (std::string key, ResourceHandle handle);
		static ResourceHandle Cache (std::string key);
		
	private:
		static AssetManager & GetInstance ()
		{
			static AssetManager instance {};
			return instance;
		}

		ResourceCache<Texture> m_textures;
		ResourceCache<ShaderSource> m_shaderSources;

		std::map<ResourceHandle, ProtoShader> m_shaderCollections;

		std::map<std::string, ResourceHandle> m_keyCache;
		std::map<ResourceHandle, std::string> m_keyCacheReversed;

		Mesh m_quadMesh;

		AssetManager ();
		AssetManager (AssetManager && AssetManager) = delete;
		AssetManager (AssetManager const & AssetManager) = delete;
		AssetManager (AssetManager & AssetManager) = delete;
		AssetManager & operator= (AssetManager && AssetManager) = delete;
		AssetManager & operator= (AssetManager & AssetManager) = delete;
		AssetManager & operator= (AssetManager const & AssetManager) = delete;

		friend class SceneEditor;
	};
}

template <> bool TLOT::Resource<TLOT::Texture>::LoadFromDisk (std::string path);

template <> bool TLOT::Resource<TLOT::ShaderSource>::LoadFromDisk (std::string path);
