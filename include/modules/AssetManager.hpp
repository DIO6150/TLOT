#pragma once

#include <stdint.h>

#include <filesystem>
#include <map>
#include <string>

#include <core/Logger.hpp>

#include <resources/Mesh.hpp>
#include <resources/Resource.hpp>
#include <resources/Shader.hpp>
#include <resources/Texture.hpp>

namespace TLOT
{
	namespace fs = std::filesystem;

	class AssetManager
	{
	public:
		AssetManager ();

		ResourceHandle LoadTexture (std::string key, fs::path path);
		ResourceHandle LoadMesh    (std::string key, fs::path path);
		ResourceHandle LoadShader  (std::string key, fs::path vs_path, fs::path fs_path); // ultimatly I should be able to load vertex shaders and fragment shaders indepently

		Texture  & GetTexture  (ResourceHandle handle);
		Mesh     & GetMesh     (ResourceHandle handle);
		Shader   & GetShader   (ResourceHandle handle);

		ResourceHandle GetQuadMeshID ();

	private:
		AssetManager (AssetManager && assetManager) = delete;
		AssetManager & operator= (AssetManager && assetManager) = delete;
		
		AssetManager (AssetManager const & assetManager) = delete;
		AssetManager (AssetManager & assetManager) = delete;
		AssetManager & operator= (AssetManager & assetManager) = delete;
		AssetManager & operator= (AssetManager const & assetManager) = delete;

		template<class Resource>
		class ResourceManager
		{
		public:
			template<class ... Args>
			ResourceHandle PushResource (std::string key, Args && ... args)
			{
				if (m_nextResourceHandle == InvalidResource)
				{
					Logger::panic ("Ran out of resources IDs (how ?)");
					return InvalidResource;
				}

				ResourceHandle handle = m_nextResourceHandle++;
				m_keyToResource.emplace (key, handle);
				m_resources.emplace (handle, std::make_unique<Resource> (std::forward<Args> (args)...));

				return handle;
			}

			Resource & GetResource (ResourceHandle handle)
			{
				assert (m_resources.find (handle) != m_resources.end ());

				return *m_resources [handle].get ();
			}

			ResourceHandle GetValue (std::string key) const
			{
				assert (m_keyToResource.find (key) != m_keyToResource.end ());

				return m_keyToResource.at (key);
			}

			bool KeyExists (std::string key) const { return m_keyToResource.find (key) != m_keyToResource.end (); }

		private:
			std::map<std::string, ResourceHandle> m_keyToResource;
			std::map<ResourceHandle, std::unique_ptr<Resource>> m_resources;

			ResourceHandle m_nextResourceHandle = 0;
		};

		ResourceManager<Texture> m_textures;
		ResourceManager<Mesh>    m_meshes;
		ResourceManager<Shader>  m_shaders;

		ResourceHandle m_quadMeshID;

		ID m_nextMeshID = 0;
		
	};
}