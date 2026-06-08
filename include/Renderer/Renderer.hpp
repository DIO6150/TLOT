#pragma once

#include <unordered_set>
#include <vector>

#include <Renderer/IndexLocation.hpp>
#include <Renderer/GeometryLocation.hpp>
#include <Renderer/RendererCommon.hpp>
#include <Renderer/Technique.hpp>
#include <OpenGL/BufferObject/DIBO.hpp>
#include <OpenGL/ShaderProgram.hpp>
#include <OpenGL/TextureAtlas/TextureAtlas.hpp>
#include <Transform.hpp>

#include <AssetManager.hpp>


namespace TLOT
{
	struct SceneObject 
	{
		ResourceHandle geometryID;
		ResourceHandle materialID;
		
		// Geometry offsets
		uint32_t indexCount;
		uint32_t firstIndex;
		int32_t  baseVertex;

		// Index in SSBO
		uint32_t matrixIndex;
		uint32_t materialIndex;

		// culling flag
		bool isVisible = true; 
	};

	class Renderer
	{
	public:
		Renderer(std::shared_ptr<VertexTemplate> vertexTemplate, size_t windowWidth, size_t windowHeight);
		
		TextureQuad Quad(ResourceHandle texture);
		ID64_t CreateProgram(ResourceHandle vertexSource, ResourceHandle fragmentSource);
		void RegisterTechnique(Technique technique);

		SceneObject Instanciate(ResourceHandle geometryID, MaterialInstance material, Transform transform);
		void KillInstance(SceneObject instance);
		void Update(SceneObject instance, MaterialInstance material, Transform transform);
		void Update(SceneObject instance, MaterialInstance material);
		void Update(SceneObject instance, Transform transform);

		void Render(SceneObject instance);

		void Render();

	private:
		struct InstanceData
		{
			uint32_t matrixIndex;
			uint32_t materialIndex;
		};

		struct FrameRenderData
		{
			std::map<ResourceHandle, std::vector<DrawCommand>> commands;
			std::vector<InstanceData> instanceDataSSBO;
		};
		
		void Sync();
		FrameRenderData BuildCommands(std::map<ResourceHandle, std::vector<SceneObject>> const & sceneObjects);

		std::map<ResourceHandle, std::vector<SceneObject>> m_toRender;
		
		IndexLocation      m_matrixLocation;
		MultiIndexLocation m_materialLocation;
		GeometryLocation   m_geometry;
		SSBO m_matrixSSBO;
		std::map<ResourceHandle, SSBO> m_materialSSBO; //material -> ssbo
		SSBO m_indexSSBO;
		std::map<ResourceHandle, BufferObject<GL_DRAW_INDIRECT_BUFFER>> m_drawCommandDIBO;

		glm::mat4 m_persp;
		glm::mat4 m_ortho;
		
		std::vector<Technique> m_techniques; // material -> technique
		std::map<ID64_t, ShaderProgram> m_programs; // program hash -> program

		TextureAtlas m_atlas;

		ID64_t m_nextInstanceID = 0;

		friend class SceneInspector;
	};
}

// 		template<class T>
//		void Update(uint32_t index, T & data)
//		{
//			m_ssbo.Push(data, index * sizeof(T));
//		}