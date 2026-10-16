
#include <glm/glm.hpp>

#include <TetoEngine/Private/Renderer/GeometryBuffer.hpp>

using namespace TetoEngine;

// TODO : verify all offset calculations

// 3pos 2uv 3normals
#define LAYOUTSIZE 8

void GeometryBuffer::create()
{
	vao_.bind();
	vbo_.create(GL_STREAM_DRAW, 10000000);
	ebo_.create(GL_STREAM_DRAW, 10000000);
	vao_.create();

	vao_.unbind();
}

void GeometryBuffer::registerGeometry(ID64 geometryID, Geometry * geometry)
{
	//if (geometry->layout_ != layout_) return;
	if (geometry_.find(geometryID) != geometry_.end()) return;

	GeometryData data;
	data.vertexBegin_ = vboCurrentSize_;
	data.vertexCount_ = geometry->vertices_.size() * sizeof(float) / LAYOUTSIZE; // geometry.vertices_.size() / (layout_.size() / sizeof(float));
	data.indexBegin_  = eboCurrentSize_;
	data.indexCount_  = geometry->indices_.size();
	geometry_.emplace(geometryID, std::move(data));

	vboCurrentSize_ += geometry->vertices_.size();
	eboCurrentSize_ += geometry->indices_ .size();

	pendingGeometry_.emplace(geometryID, geometry);
	dirty_ = true;
}

void GeometryBuffer::upload()
{
	if(!dirty_) return;

	for (auto & [geometryID, geometry] : pendingGeometry_)
	{
		auto offsets = geometry_.at(geometryID);
		size_t vboByteOffset = offsets.vertexBegin_ * sizeof(float);
		size_t eboByteOffset = offsets.indexBegin_  * sizeof(uint32_t);

		vbo_.upload(geometry.vertices_, vboByteOffset);
		ebo_.upload(geometry.indices_ , eboByteOffset);
	}

	dirty_ = false;
}

size_t GeometryBuffer::getFirstVertex(ID64 geometryID)
{
	return geometry_.at(geometryID).vertexBegin_ * sizeof(float) / LAYOUTSIZE; // vertexBegin_ / (layout_.size() / sizeof(float))
}

size_t GeometryBuffer::getFirstIndex (ID64 geometryID)
{
	return geometry_.at(geometryID).indexBegin_;
}

size_t GeometryBuffer::getIndexCount (ID64 geometryID)
{
	return geometry_.at(geometryID).indexCount_;
}

void GeometryBuffer::bind()
{
	vao_.bind();
	vbo_.bind();
	ebo_.bind();
}

void GeometryBuffer::unbind()
{
	vao_.unbind();
	vbo_.unbind();
	ebo_.unbind();
}
