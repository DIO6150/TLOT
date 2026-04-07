#pragma once

#include <stdint.h>

#include <map>
#include <set>
#include <vector>

#include <core/DrawCommandBuffer.hpp>
#include <core/GeometryIndexBuffer.hpp>
#include <core/InstanceOffsetBuffer.hpp>
#include <core/InstanceIndexBuffer.hpp>
#include <core/TextureOffset.hpp>
#include <core/TextureIndexBuffer.hpp>
#include <core/Transform.hpp>
#include <core/Vertex.hpp>

#include <resources/Mesh.hpp>
#include <resources/Shader.hpp>
#include <resources/Texture.hpp>

#include <opengl/TextureAtlas.hpp>
#include <opengl/DrawCommand.hpp>
#include <opengl/VAO.hpp>
#include <opengl/VBO.hpp>
#include <opengl/EBO.hpp>
#include <opengl/DIBO.hpp>
#include <opengl/SSBO.hpp>

#include <modules/AssetManager.hpp>
#include <modules/Camera.hpp>

namespace TLOT
{
	class Renderer
	{
	public:
		Renderer ();

		GeometryID RegisterGeometry (Mesh const & mesh);
		InstanceID RegisterInstance (GeometryID id, Transform const transform, Material const material, AssetManager & assetManager);

		void RegisterTexture (ResourceHandle handle, Texture const & texture);
		
		void UpdateInstanceMaterial (InstanceID id, Material const & material, AssetManager & assetManager);
		void UpdateInstanceTransform (InstanceID id, Transform const & transform);

		void UnregisterInstance (InstanceID id);

		void Render (Shader const & shader, Camera const & camera);

	private:
		void SyncGPU ();

		struct TextureOffsetCollection
		{
			size_t index;
			std::vector<TextureOffset> textures;
		};

		struct Geometry
		{
			std::vector<Vertex>   const vertices;
			std::vector<uint32_t> const indices;
		};

		struct alignas(16) InstanceMaterial
		{
			glm::vec3 color;
			uint32_t diffuseTextureIndex;
			uint32_t diffuseTextureCount;
		};

		struct alignas(16) Instance
		{
			glm::mat4 model;
			InstanceMaterial mat;
		};

		size_t s = sizeof (Instance);

		InstanceMaterial CreateInstanceMaterial (Material const & material, AssetManager & assetManager);

		GeometryID m_nextGeometryID = 0;
		InstanceID m_nextInstanceID = 0;

		std::vector<std::pair<GeometryID, Geometry>> m_geometryToSend;
		std::vector<std::pair<size_t, Instance>> m_instanceToSend;
		std::map<Hash, TextureOffsetCollection> m_texturesOffsetToSend;
		std::vector<std::pair<size_t, DrawCommand>> m_commandsToSend;

		std::vector<std::pair<size_t, InstanceMaterial>> m_materialToUpdate;
		std::vector<std::pair<size_t, glm::mat4>> m_transformToUpdate;

		std::map<ID, GeometryID> m_registeredGeometry;
		GeometryIndexBuffer      m_geometryIndexBuffer;
		
		std::set<InstanceID> m_registerdInstances;
		InstanceOffsetBuffer m_instanceOffsetBuffer;
		InstanceIndexBuffer  m_instanceIndexBuffer;
		std::map<InstanceID, GeometryID> m_instanceToGeometry;

		TextureIndexBuffer m_textureIndexBuffer;
		
		DrawCommandBuffer m_drawCommandBuffer;

		TextureAtlas m_atlas;

		glm::mat4 m_projection = glm::perspective (glm::radians (45.0f), 1.0f, 0.1f, 100.0f);

		
		constexpr static const size_t s_initialVertexCount    = 1000000;
		constexpr static const size_t s_initialIndexCount     = 1000000;
		constexpr static const size_t s_initialInstanceCount  = 10000;

		VAO  m_vao;
		VBO  m_vbo  = {s_initialVertexCount  , GL_DYNAMIC_DRAW};
		EBO  m_ebo  = {s_initialIndexCount   , GL_DYNAMIC_DRAW};
		DIBO m_dibo = {s_initialInstanceCount, GL_DYNAMIC_DRAW};
		SSBO<uint32_t>      m_ssboInstanceIndex = {s_initialInstanceCount, GL_DYNAMIC_DRAW, 0};
		SSBO<Instance>      m_ssboInstanceData  = {s_initialInstanceCount, GL_DYNAMIC_DRAW, 1};
		SSBO<TextureOffset> m_ssboTextureOffset = {s_initialInstanceCount, GL_DYNAMIC_DRAW, 2};
		
	};
}