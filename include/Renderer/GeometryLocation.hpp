#pragma once

#include <map>

#include <Core/ID.hpp>
#include <Core/ResourceHandle.hpp>
#include <OpenGL/BufferObject.hpp>
#include <OpenGL/VAO.hpp>
#include <Geometry.hpp>


namespace TLOT
{
	class GeometryLocation
	{
	public:
		void Create(std::shared_ptr<VertexTemplate> vertexTemplate);
		void RegisterGeometry(ResourceHandle geometryID, Geometry const & geometry);

		void Upload();
		
		void Bind();
		void Unbind();


		size_t GetFirstVertex(ResourceHandle geometryID);

		size_t GetFirstIndex (ResourceHandle geometryID);
		size_t GetIndexCount (ResourceHandle geometryID);

	private:
		struct GeometryData
		{
			size_t vertexBegin;
			size_t vertexCount;

			size_t indexBegin;
			size_t indexCount;
		};

		std::shared_ptr<VertexTemplate> m_template; // maybe allow multiple templates

		VAO m_vao;
		BufferObject<GL_ARRAY_BUFFER>         m_vbo;
		BufferObject<GL_ELEMENT_ARRAY_BUFFER> m_ebo;

		std::map<ResourceHandle, GeometryData> m_geometry;
		std::map<ResourceHandle, Geometry>     m_toUpload;

		size_t m_vboCurrentSize = 0;
		size_t m_eboCurrentSize = 0;

		bool m_upToDate = false;
	};
}