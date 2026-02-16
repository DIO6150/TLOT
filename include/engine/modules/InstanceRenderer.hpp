#pragma once

#include <engine/utils/Types.hpp>
#include <engine/utils/ShortcutTypes.hpp>

#include <engine/core/Renderer.hpp>
#include <engine/core/ResourceManager.hpp>

#include <engine/modules/Camera.hpp>

#include <engine/data/Texture.hpp>


namespace Engine::Internal {
	class PostProcessingEffect {
	public:
		PostProcessingEffect (Core::ShaderID shader);

		Core::ShaderID postProcess;
		bool active;
	};

	class FrameBuffer {
	public:
		FrameBuffer ();
		FrameBuffer (int width, int height);
		~FrameBuffer ();

		FrameBuffer (FrameBuffer && other) = default;
		FrameBuffer (FrameBuffer & other) = default;
		FrameBuffer (const FrameBuffer & other) = default;

		void Init (int width, int height);

		unsigned int Get () const;
		unsigned int GetColorAttachement () const;
	private:
		unsigned int mFrameBuffer;
		unsigned int mTextureColorBuffer;
		unsigned int mRenderBuffer;
	};
}

namespace Engine::Module {
	class RenderingGroup {
	public:
		RenderingGroup (Core::ShaderID shader, const Core::SceneGraph * graph, const Core::AssetManager * manager, const Data::TextureAtlas * atlas);
		void Init ();
		void UploadInstance ();
		void UploadBuffers ();
		void GenerateDrawCommands ();

		void Bind ();
		void Unbind ();

	
	private:
		void GenerateAttributes ();
		glm::vec4 CoordsRatio (const Data::Mesh * mesh) const;

		// Buffers
		uint32_t vao;
		uint32_t vbo;
		uint32_t ibo;
		uint32_t dibo;
		uint32_t ssbo;

		// Buffer population
		size_t max_vertex_count;
		size_t max_index_count;

		// cache
		ObjectMap<Vector<Data::Mesh *>>		geometry_to_mesh;
		
		Vector<Core::GeometryID>		geometries;
		Vector<Core::DrawCommand>		commands;

		Core::ShaderID				shader;

		bool					dirty;

		const Core::SceneGraph   * pScene;
		const Core::AssetManager * pManager;
		const Data::TextureAtlas * pAtlas;

		friend class InstanceRenderer;
	};

	class InstanceRenderer: public Core::IRenderer {
	public:
		InstanceRenderer (Core::Scene * scene);
		~InstanceRenderer ();

		void AddPostProcessingEffect (std::string name, Core::ShaderID shader);
		void DisablePostProcessingEffect (std::string name);
		void RemovePostProcessingEffect (std::string name);

		void Init (int width, int height) override;
		void Render (Camera * camera) override;

	private:
		std::vector<RenderingGroup> mGroups;
		Data::TextureAtlas mAtlas;

		std::vector<std::pair<std::string, Internal::PostProcessingEffect>> mEffects;
		Core::ShaderID mFinalBlit;
		
		Internal::FrameBuffer mSceneFrameBuffer;
		std::vector<Internal::FrameBuffer> mPingPong;

		unsigned int mFBVAO;
		unsigned int mFBVBO;
	};
}
