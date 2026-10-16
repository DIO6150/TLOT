#pragma once

#include <map>

#include <TetoEngine/Common/ID.hpp>

#include <TetoEngine/Private/Renderer/Buffer.hpp>
#include <TetoEngine/Private/Renderer/VAO.hpp>

#include <TetoEngine/Resource/Geometry.hpp>

namespace TetoEngine
{
	class GeometryBuffer
	{
	public:
		void create();
		void registerGeometry(ID64 geometryID, Geometry * geometry);

		void upload();
		
		void bind();
		void unbind();


		size_t getFirstVertex(ID64 geometryID);

		size_t getFirstIndex(ID64 geometryID);
		size_t getIndexCount(ID64 geometryID);

	private:
		struct GeometryData
		{	
			size_t vertexBegin_;
			size_t vertexCount_;

			size_t indexBegin_;
			size_t indexCount_;
		};

		VAO vao_;
		Buffer<GL_ARRAY_BUFFER>         vbo_;
		Buffer<GL_ELEMENT_ARRAY_BUFFER> ebo_;

		std::map<ID64, GeometryData> geometry_;
		std::map<ID64, Geometry>     pendingGeometry_;

		size_t vboCurrentSize_ = 0;
		size_t eboCurrentSize_ = 0;

		bool dirty_ = false;
	};
}
