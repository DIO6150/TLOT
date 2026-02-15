#pragma once

#include <stdint.h>

#include <stdexcept>
#include <vector>

#include <engine/core/AssetManager.hpp>
#include <engine/core/ResourceManager.hpp>
#include <engine/core/SceneGraph.hpp>
#include <engine/data/Texture.hpp>

#include <engine/utils/Types.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Engine::Core {
	class Scene {
	public:
		Scene ();
		void Load (const std::string & url);
	
		AssetManager	asset_manager;
		SceneGraph	graph;
		
		ShaderID	defaultShader;
		MaterialID	defaultMaterial;
		TextureID	defaultTexture;
		
	private:
		void ProcessMesh (aiMesh * mesh,  const aiScene * scene, const aiMatrix4x4 & transform, const std::string & directory, SceneNode * node);
		void ProcessNode (aiNode * node, const aiScene * scene, const std::string & directory, SceneNode * graph_node);
	};
}
