#pragma once

#include <set>
#include <unordered_set>
#include <optional>
#include <vector>

#include <TetoEngine/Common/Material.hpp>
#include <TetoEngine/Common/Transform.hpp>
#include <TetoEngine/Common/VertexLayout.hpp>
#include <TetoEngine/Common/ID.hpp>

#include <TetoEngine/Renderer/TRID.hpp>
#include <TetoEngine/Renderer/Technique.hpp>

#include <TetoEngine/Private/Renderer/TextureAtlas.hpp>
#include <TetoEngine/Private/Renderer/GlyphAtlas.hpp>
#include <TetoEngine/Private/Renderer/Glyph.hpp>
#include <TetoEngine/Private/Renderer/InstanceIndexBuffer.hpp>

#include <TetoEngine/Private/GPUResources/ShaderProgram.hpp>
#include <TetoEngine/Private/GPUResources/OpenGLShaderStorageBuffer.hpp>
#include <TetoEngine/Private/GPUResources/GeometryBuffer.hpp>
#include <TetoEngine/Private/GPUResources/DrawCommand.hpp>

#include <glm/glm.hpp>


namespace TetoEngine
{
	class Application;

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

	class InstanceRenderer
	{
	public:

		InstanceRenderer(Application * app);

		void registerTechnique(Technique technique);
		TRID registerTexture(ID64 handle);
		
		TRID createFont(ID64 handle);
		TRID generateProgram(ID64 vertexHandle, ID64 fragmentHandle);

		std::optional<Material> generateMaterial(ID64 handle);


		TetoInstance instanciate(ID64 handle, glm::mat4 model, Material material);
		TetoInstance instanciate(ID64 handle);
		void updateInstance(TetoInstance instance, glm::mat4 model, Material material);
		void updateInstance(TetoInstance instance, glm::mat4 model);
		void updateInstance(TetoInstance instance, Material material);
		void drawInstance(TetoInstance instance);
		void killInstance(TetoInstance & instance);

		void render();

	private:
		void init();
		void sync();

		FrameRenderData buildCommands(std::map<ID64, std::vector<TetoInstance>> const & objects);

		void createMaterialBuffer(ID64 handle);

	private:
		/* Draw List */
		std::map<ID64, std::vector<TetoInstance>> renderList_;
		
		
		/* Render Buffers */
		// Buffers should be shared between renderer I think (like having something generic like a GPU resource instead)
		// ^next thing I do after finishing the TextureAtlas
		InstanceIndexBuffer      matrixIndexBuffer_;
		MultiInstanceIndexBuffer materialIndexBuffer_;
		GeometryBuffer geometryBuffer_;

		std::map<ID64, OpenGLBuffer<GL_DRAW_INDIRECT_BUFFER>> drawCommandBuffer_;
		ShaderStorageBuffer indexSSBO_;
		ShaderStorageBuffer matrixSSBO_;
		std::map<TRID, ShaderStorageBuffer> materialSSBO_;


		/* Projection */
		glm::mat4 perspective_;
		glm::mat4 ortho_;


		/* Atlases */
		TextureAtlas textureAtlas_;
		GlyphAtlas   fontAtlas_ {2048, 2048};
		

		/* Registed Objects */
		// Or just object the renderer knows of
		std::vector<Technique> techniqueList_;
		std::map<ID64, ShaderProgram> programList_;
		std::map<ID64, std::polymorphic<MaterialTemplate>> materialTemplates_;
		std::set<ID64> registeredMaterial_;

		//When creating render object that needs a unique ID increment this
		TRID nextTetoID_ = 0;

	private:
		Application * app_;
	};
}
