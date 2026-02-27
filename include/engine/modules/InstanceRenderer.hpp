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
		HandleID shaderID;

		HandleID verticesID;
		HandleID indicesID;

		Instance<Material> material;

		glm::mat4 transform;
		
		bool visible;
	};
}

namespace Engine::Module {
	class InstanceRenderer: public Core::IRenderer {
	public:
		void Render (Camera * camera) override;

		void PushModel (Instance<Model> & model, const HandleID & shader) override;
		
		void AddPostProcessingEffect		(std::string name, Core::HandleID shader);
		void AddPostProcessingEffect		(std::string name, Core::HandleID shader, size_t colorAttachmentCount, Data::ShaderInputs && params);
		void DisablePostProcessingEffect	(std::string name);
		void RemovePostProcessingEffect		(std::string name);
		
		InstanceRenderer (int32_t width, int32_t height);
		~InstanceRenderer ();
		
	private:
		void Clear ();

		std::set<uint64_t> mInstances;
		std::vector<Internal::RenderCommand> mCommands;

		HandleMap<Internal::RenderingGroup> mGroups;
		Data::TextureAtlas mAtlas;

		Vector<std::pair<std::string, Internal::PostProcessingEffect>> mEffects;
		Core::HandleID mFinalBlit;
		
		Internal::FrameBuffer mSceneFrameBuffer;
		Vector<Internal::FrameBuffer> mPingPong;
		size_t mMaxColorAttachment;

		unsigned int mFBVAO;
		unsigned int mFBVBO;
	};
}

namespace Engine::Internal {
	struct InstanceRenderData {
		glm::mat4 transform;
		glm::vec4 textureOffset_0; // supposedly we could have multiple textures, but how can we add an arbitary number of textures ?
		glm::vec4 color;
	};

	// TODO: make rendering groups dynamically resizable
	class RenderingGroup {
	public:
		RenderingGroup (Data::TextureAtlas * atlas);

		void Push (const RenderCommand & command);

		void UploadDrawCommands ();
		void UploadInstances ();

		void ClearInstances ();

		void Bind ();
		void Unbind ();

		uint32_t GetCommandsCount();

		bool ShouldRegenerateTexture ();
		void SetTextureRegenerate (bool flag);
	
	private:
		void GenerateAttributes ();
		glm::vec4 CoordsRatio (const Instance<Material> & material) const;

		// Buffers
		uint32_t vao;
		uint32_t vbo;
		uint32_t ibo;
		uint32_t dibo;
		uint32_t ssbo;

		// Buffer population
		size_t mCurrentVertexCount;
		size_t mCurrentIndexCount;

		// cache
		bool mDirtyTextures;

		std::set<HandleID> mKnownGeometry; // keeps track of verticesID added to the DrawCommand buffer (gpu side)

		std::map<HandleID, size_t> mGeometryToCommands; // link verticesID to a command in the DrawCommands vector
		std::vector<DrawCommand> mDrawCommands;

		std::map<HandleID, std::vector<InstanceRenderData>> mInstances;


		Data::TextureAtlas * pAtlas;
	};
}