#include <Renderer/GeometryLocation.hpp>

using namespace TLOT;

void GeometryLocation::Create(std::shared_ptr<VertexTemplate> vertexTemplate)
{
	m_template = vertexTemplate;
	m_vao.Bind();
	m_vbo.Create(GL_STREAM_DRAW, 10000000);
	m_ebo.Create(GL_STREAM_DRAW, 10000000);
	m_vao.Create(vertexTemplate);

	m_vao.Unbind();
}

void GeometryLocation::RegisterGeometry(ResourceHandle geometryID, Geometry const & geometry)
{
	if (geometry.vertexTemplate != m_template) return;
	if (m_geometry.find(geometryID) != m_geometry.end()) return;

	GeometryData data;
	data.vertexBegin = m_vboCurrentSize;
	data.vertexCount = geometry.vertices.size() / (geometry.vertexTemplate->Size() / sizeof(float));
	data.indexBegin  = m_eboCurrentSize;
	data.indexCount  = geometry.indices.size();
	m_geometry.emplace(geometryID, std::move(data));

	m_vboCurrentSize += geometry.vertices.size();
	m_eboCurrentSize += geometry.indices .size();

	m_toUpload[geometryID] = geometry;

	m_upToDate = false;
}

void GeometryLocation::Upload()
{
	if(m_upToDate) return;

	for (auto & [geometryID, geometry] : m_toUpload)
	{
		auto offsets = m_geometry.at(geometryID);
		size_t vboByteOffset = offsets.vertexBegin;
		size_t eboByteOffset = offsets.indexBegin;

		m_vbo.Upload(geometry.vertices, vboByteOffset);
		m_ebo.Upload(geometry.indices , eboByteOffset);
	}

	m_upToDate = true;
}

size_t GeometryLocation::GetFirstVertex(ResourceHandle geometryID)
{
	return m_geometry.at(geometryID).vertexBegin;
}

size_t GeometryLocation::GetFirstIndex (ResourceHandle geometryID)
{
	return m_geometry.at(geometryID).indexBegin;
}

size_t GeometryLocation::GetIndexCount (ResourceHandle geometryID)
{
	return m_geometry.at(geometryID).indexCount;
}

void GeometryLocation::Bind()
{
	m_vao.Bind();
	m_vbo.Bind();
	m_ebo.Bind();
}

void GeometryLocation::Unbind()
{
	m_vao.Unbind();
	m_vbo.Unbind();
	m_ebo.Unbind();
}
