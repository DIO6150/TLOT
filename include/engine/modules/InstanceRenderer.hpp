#pragma once

#include <engine/utils/ShortcutTypes.hpp>
#include <engine/utils/Types.hpp>

#include <engine/opengl/FrameBuffer.hpp>
#include <engine/core/Renderer.hpp>
#include <engine/core/ResourceManager.hpp>
#include <engine/core/PostProcessing.hpp>

#include <engine/modules/Camera.hpp>

#include <engine/data/ShaderInputs.hpp>
#include <engine/data/Texture.hpp>
#include <engine/data/InstancedData.hpp>

#include <functional>
#include <map>
#include <set>

namespace Engine::Internal {
	struct RenderCommand {
		uint64_t instanceID;

		uint32_t vertexOffset;
		uint32_t indexOffset;

		HandleID verticesID;
		HandleID indicesID;

		glm::mat4 transform;
		glm::vec4 textureCoord;
		glm::vec4 color;
	};

	struct GeometryData {
		HandleID indicesID;

		uint32_t offsetVertex;
		uint32_t offsetIndex;
	};

	struct GeometryBufferCPU {
		GeometryBufferCPU ():
			mCurrentVertex {0},
			mCurrentIndex  {0} {

		}

		uint32_t GetVertexOffset (HandleID verticesID) {
			return mGeometryBuffer.at (verticesID).offsetVertex;
		}

		uint32_t GetIndexOffset (HandleID verticesID) {
			return mGeometryBuffer.at (verticesID).offsetIndex;
		}
		
		void insert (HandleID verticesID, HandleID indicesID) {
			mGeometryBuffer.emplace (verticesID, GeometryData {indicesID, mCurrentVertex, mCurrentIndex});

			auto am = AssetManager::GetInstance ();
			mCurrentVertex += am->GetVerticesSize (verticesID);
			mCurrentIndex  += am->GetIndicesSize  (indicesID);
		}
		auto find  (const HandleID & verticesID) { return mGeometryBuffer.find (verticesID); }
		auto begin () { return mGeometryBuffer.begin (); }
		auto end   () { return mGeometryBuffer.end   (); }

	private:
		std::unordered_map<HandleID, GeometryData> mGeometryBuffer;
		uint32_t mCurrentVertex;
		uint32_t mCurrentIndex;
	};
}

namespace Engine::Module {
	class InstanceRenderer: public Core::IRenderer {
	public:
		void Render (Camera * camera) override;

		void RegisterGeometry (const Model & model);
		void RegisterGeometry (const HandleID & vertices, const HandleID & indices);

		void PushModel (Instance<Model> & model, const HandleID & shader) override;
		void UpdateModelTransform (Instance<Model> & model) override;
		
		void AddPostProcessingEffect     (std::string name, Core::HandleID shader);
		void AddPostProcessingEffect     (std::string name, Core::HandleID shader, size_t colorAttachmentCount, Data::ShaderInputs && params);
		void DisablePostProcessingEffect (std::string name);
		void RemovePostProcessingEffect  (std::string name);
		
		InstanceRenderer (int32_t width, int32_t height);
		~InstanceRenderer ();
		
	private:
		void Clear ();

		unsigned int vao;
		unsigned int vbo;
		unsigned int ebo;

		// Buffer population
		Internal::GeometryBufferCPU mGeometryBuffer;

		std::unordered_map<uint64_t, HandleID> mMeshInstancesToShader;
		std::set<uint64_t> mInstances;

		HandleMap<Internal::RenderingGroup> mGroups;
		Data::TextureAtlas mAtlas;

		Vector<std::pair<std::string, Internal::PostProcessingEffect>> mEffects;
		Core::HandleID mFinalBlit;
		
		Internal::FrameBuffer mSceneFrameBuffer;
		Vector<Internal::FrameBuffer> mPingPong;
		size_t mMaxColorAttachment;

		unsigned int mFBVAO;
		unsigned int mFBVBO;

		unsigned long long int mDebugMeshCounter;
	};
}

namespace Engine::Internal {
	struct InstanceRenderData {
		glm::mat4 transform;
		glm::vec4 textureOffset_0; // supposedly we could have multiple textures, but how can we add an arbitary number of textures ?
		glm::vec4 color;
	};

	struct InstanceWrapper {
		InstanceRenderData data;
		uint32_t position;
	};

	// TODO: make rendering groups dynamically resizable
	class RenderingGroup {
	public:
		RenderingGroup ();

		void Push (const RenderCommand & command);
		
		void UploadDrawCommands ();
		void UploadInstances ();
		
		void UpdateInstanceTransform (uint64_t meshID, glm::mat4 transform);

		void Bind ();
		void Unbind ();
		
		uint32_t GetCommandsCount ();

		void debugUpdate () {++mCounter;}
		
	private:
		void CreateDrawCommand (const HandleID & verticesID, const std::vector<Vertex> & vertices, const std::vector<uint32_t> indices, const uint32_t vertexOffset, const uint32_t indexOffset);
		void DecreaseDCInstance (const HandleID & verticesID);


		// Buffers
		uint32_t dibo;
		uint32_t ssbo;


		std::map<HandleID, size_t> mGeometryToCommands; // link verticesID to a command in the DrawCommands vector
		std::vector<HandleID>      mCommandsToGeometry; // link a command to its geometry id
		std::vector<DrawCommand>         mDrawCommands;
		
		uint32_t mDirtyCommand; // the index of the lowest command id that was modified


		// TODO: all of this could be put in a struct, to not pollute too much RenderingGroup
		std::multimap<HandleID, uint64_t> mGeometryToInstances;
		std::map<uint64_t, HandleID>      mInstancesToGeometry;
		std::map<uint64_t, InstanceRenderData>      mInstances;

		std::map<uint32_t, uint64_t> mPositionToInstance;
		std::map<uint64_t, uint32_t> mInstanceToPosition;
		std::set<uint64_t> mDirtyInstances;

		std::multimap<HandleID, uint32_t> mFreePositions;
		std::map<HandleID, uint32_t>   mNextFreePosition;


		unsigned long long int mCounter;


	};
}
