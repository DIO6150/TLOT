#pragma once

#include <engine/core/ResourceManager.hpp>

#include <engine/utils/Types.hpp>

#include <engine/data/Material.hpp>
#include <engine/data/Mesh.hpp>
#include <engine/data/Model.hpp>
#include <engine/data/Shader.hpp>
#include <engine/data/Texture.hpp>
#include <engine/data/Vertex.hpp>

#include <engine/utils/ShortcutTypes.hpp>

#include <optional>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Engine::Internal {
	template<class T>
	class RefMap {
	public:
		void add (const String & name, const T & ref) {
			t_to_str[ref]	= name;
			str_to_t[name]	= ref;
		}

		T * get_ptr (const String & name) const {
			const auto & pos = str_to_t.find (name);

			if (pos != str_to_t.end ()) {
				return const_cast<T *> (&pos->second);
			}

			return nullptr;
		}

		const String * get_name (const T & ref) const {
			const auto & pos = t_to_str.find (ref);

			if (pos != t_to_str.end ()) {
				return const_cast<String *> (&pos->second);
			}

			return nullptr;
		}

	private:
		std::unordered_map<T, String> t_to_str;
		std::unordered_map<String, T> str_to_t;
	};
}

namespace Engine::Core {
	// in the future, resources like shaders shouldnt know about their own url and should instead receive raw data (except maybe shaders for hot reloading ? or maybe not...)
	class AssetManager {
	public:
		static AssetManager * GetInstance ();

		HandleID LoadShader  (const String & name, const String & url_vertex, const String & url_fragment);
		HandleID LoadTexture (const String & name, const String & url);
		HandleID LoadModel   (const String & name, const String & url);


		HandleID CreateMaterial (const String & name, Vector<HandleID> diffuse, glm::vec3 color);


		HandleID GetShaderID   (const String & name) const;
		HandleID GetTextureID  (const String & name) const;
		HandleID GetMaterialID (const String & name) const;
		HandleID GetMeshID     (const String & name) const;
		HandleID GetModelID    (const String & name) const;


		const std::string GetTextureName (const HandleID & name) const;
		
		
		Data::Shader   * GetShader   (const HandleID & shaderID)   const;
		Data::Texture  * GetTexture  (const HandleID & textureID)  const;
		Data::Material * GetMaterial (const HandleID & materialID) const;
		Data::Mesh     * GetMesh	   (const HandleID & meshID)     const;
		Data::Model    * GetModel	   (const HandleID & modelID)    const;

		
		std::vector<Data::Vertex> GetVertices (HandleID verticesID);
		std::vector<uint32_t>     GetIndices  (HandleID indicesID);

		uint32_t GetVerticesSize (HandleID verticesID);
		uint32_t GetIndicesSize  (HandleID indicesID);

		

	private:

		AssetManager ();
		AssetManager (const AssetManager & other) = delete;
		AssetManager (AssetManager & other)       = delete;
		AssetManager (AssetManager && other)      = delete;

		~AssetManager () = default;


		bool operator= (const AssetManager & other)  = delete;
		bool operator= (const AssetManager && other) = delete;
		bool operator= (AssetManager & other)        = delete;
		bool operator= (AssetManager && other)       = delete;

		static AssetManager * mInstance;

		CompoundElementArray<Data::Vertex> mVerticesIDs;
		CompoundElementArray<uint32_t>     mIndicesIDs;

		ResourceManager<Data::Shader>	shaderIDs;
		Internal::RefMap<HandleID>	shaderNames;

		ResourceManager<Data::Texture>	textureIDs;
		Internal::RefMap<HandleID>	textureNames;

		ResourceManager<Data::Material>	materialIDs;
		Internal::RefMap<HandleID>	materialNames;
		
		ResourceManager<Data::Mesh>	meshIDs;
		Internal::RefMap<HandleID>	meshNames;

		ResourceManager<Data::Model>	mModelIDs;
		Internal::RefMap<HandleID>	mModelNames;

		HandleID defaultTextureID;
		HandleID missingTextureID;
		HandleID defaultShaderID;
		HandleID defaultMaterialID;

		HandleID ProcessMesh (aiMesh * assimpMesh, aiMaterial * assimpMaterial, const std::string & directory);
	};
}
