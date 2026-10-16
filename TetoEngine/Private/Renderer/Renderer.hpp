#pragma once

#include <set>
#include <unordered_set>
#include <vector>

#include <TetoEngine/Common/Material.hpp>
#include <TetoEngine/Common/Transform.hpp>
#include <TetoEngine/Common/VertexLayout.hpp>


#include <TetoEngine/Common/ID.hpp>

#include <TetoEngine/Renderer/TRID.hpp>
#include <TetoEngine/Renderer/Technique.hpp>

#include <TetoEngine/Private/Renderer/TextureAtlas.hpp>
#include <TetoEngine/Private/Renderer/GlyphAtlas.hpp>
#include <TetoEngine/Private/Renderer/ShaderProgram.hpp>
#include <TetoEngine/Private/Renderer/Glyph.hpp>
#include <TetoEngine/Private/Renderer/GeometryBuffer.hpp>
#include <TetoEngine/Private/Renderer/InstanceIndexBuffer.hpp>
#include <TetoEngine/Private/Renderer/ShaderStorageBuffer.hpp>

#include <glm/glm.hpp>


namespace TetoEngine
{
	class Application;

	struct DrawCommand
	{
		uint32_t indexCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t  baseVertex;
		uint32_t baseInstance;
	};

	struct InstanceData
	{
		uint32_t matrixIndex;
		uint32_t materialIndex;
	};

	struct FrameRenderData
	{
		std::map<ID64, std::vector<DrawCommand>> commands;
		std::vector<InstanceData> instanceDataSSBO;
	};

	class Renderer
	{
	public:

		Renderer(Application * app);

		void registerTechnique(Technique technique);
		TRID registerTexture(ID64 handle);
		
		TRID createFont(ID64 handle);
		TRID createProgram(ID64 vertexHandle, ID64 fragmentHandle);

		Material generateMaterial(ID64 handle);


		TIRID instanciate(ID64 handle, glm::mat4 model, Material material);
		TIRID instanciate(ID64 handle);
		void updateInstance(TIRID instance, glm::mat4 model, Material material);
		void updateInstance(TIRID instance, glm::mat4 model);
		void updateInstance(TIRID instance, Material material);
		void drawInstance(TIRID instance);
		void killInstance(TIRID & instance);

		void render();

	private:
		void init();
		void sync();

		TRID getProgramHash(ResourceView<ShaderSource> vertex, ResourceView<ShaderSource> fragment);
		FrameRenderData buildCommands(std::map<ID64, std::vector<TIRID>> const & objects);

		void createMaterialBuffer(ID64 handle);

	private:
		Application * app_;

		std::map<ID64, std::vector<TIRID>> renderList_;

		InstanceIndexBuffer      matrixIndexBuffer_;
		MultiInstanceIndexBuffer materialIndexBuffer_;
		GeometryBuffer geometryBuffer_;
		std::map<ID64, Buffer<GL_DRAW_INDIRECT_BUFFER>> drawCommandBuffer_;

		ShaderStorageBuffer indexSSBO_;
		ShaderStorageBuffer matrixSSBO_;
		std::map<TRID, ShaderStorageBuffer> materialSSBO_;



		glm::mat4 perspective_;
		glm::mat4 ortho_;



		TextureAtlas textureAtlas_;
		GlyphAtlas   fontAtlas_ {2048, 2048};
		


		std::vector<Technique> techniqueList_;
		std::map<ID64, ShaderProgram> programList_;
		std::map<ID64, std::shared_ptr<MaterialTemplate>> materialTemplates_;
		std::set<ID64> registeredMaterial_;

		TRID nextTetoID_ = 0;
	};
}
