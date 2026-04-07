#pragma once

#include <stdint.h>

#include <unordered_map>

#include <resources/Resource.hpp>


namespace TLOT
{
	class GeometryIndexBuffer {
	private:
		struct GeometryData
		{
			uint32_t offsetVertex;
			uint32_t offsetIndex;
			size_t   indexSize;
		};

	public:
		GeometryIndexBuffer () {}

		uint32_t GetVertexOffset (ID const geometryID)
		{
			return m_geometryBuffer.at (geometryID).offsetVertex;
		}

		uint32_t GetIndexOffset (ID const geometryID) 
		{
			return m_geometryBuffer.at (geometryID).offsetIndex;
		}

		size_t GetIndexCount (ID const geometryID)
		{
			return m_geometryBuffer.at (geometryID).indexSize;
		}
		
		void Insert (ID const geometryID, uint32_t const vertexSize, uint32_t const indexSize)
		{
			m_geometryBuffer.emplace (geometryID, GeometryData {m_currentVertIndex, m_currentIndIndex, indexSize});
			m_currentVertIndex += vertexSize;
			m_currentIndIndex  += indexSize;
		}

		
		auto find  (ID const geometryID) { return m_geometryBuffer.find (geometryID); }
		auto begin () { return m_geometryBuffer.begin (); }
		auto end   () { return m_geometryBuffer.end   (); }

	private:
		std::unordered_map<ID, GeometryData> m_geometryBuffer;
		uint32_t m_currentVertIndex = 0;
		uint32_t m_currentIndIndex  = 0;
	};
}
