#pragma once

#include <stdint.h>

#include <stdexcept>
#include <vector>

#include <engine/core/AssetManager.hpp>
#include <engine/core/DrawCommand.hpp>
#include <engine/core/ResourceManager.hpp>

#include <engine/utils/Types.hpp>

#include <engine/data/Geometry.hpp>
#include <engine/data/Instance.hpp>
#include <engine/data/Mesh.hpp>
#include <engine/data/Shader.hpp>
#include <engine/data/Texture.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Core {
	class SceneGraph;
	
	class SceneNode {
	public:
		~SceneNode ();

		void addMesh (Core::GeometryID geometry, Core::MaterialID material);
		const std::vector<std::unique_ptr<Data::Mesh>> & getMeshes ();
		const std::vector<SceneNode *> & getChildren ();
	
	private:
		SceneNode (std::string name, SceneNode * parent, SceneGraph * graph);
		template<typename F>
		void Traverse (F callback) {
			callback (meshes);
			for (auto & child : children) {
				child->Traverse<F> (callback);
			}
		}

		std::string				 name;
		std::vector<std::unique_ptr<Data::Mesh>> meshes;
		
		std::vector<SceneNode *> children;
		SceneNode *              parent;
		SceneGraph *		 graph;

		friend class SceneGraph;
		friend class Core::Scene;
	};

	class SceneGraph {
	public:
		SceneGraph ();
		~SceneGraph ();

		SceneNode * GetRoot ();

		uint32_t GetTotalMeshCount () const;
		uint32_t GetInstanceCount (Core::GeometryID geometry) const;
		uint32_t GetGeometryCount () const;

		template<typename F>
		void Traverse (F callback) {
			root.Traverse<F> (callback);
		}

		std::unordered_map<Core::GeometryID, std::vector<Data::Mesh *>> GetMeshes () const;

	private:
		SceneNode root;

		ObjectMap<Vector<Data::Mesh *>> geometry_to_meshes;
		uint32_t			mesh_count;

		friend class SceneNode;
	};
}