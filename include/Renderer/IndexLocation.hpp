#pragma once

#include <map>
#include <set>

#include <glm/glm.hpp>

#include <OpenGL/SSBO.hpp>
#include <Renderer/RendererCommon.hpp>


namespace TLOT
{
	class IndexLocation
	{
	public:
		IndexLocation();
		uint32_t Create();
		void Destroy(uint32_t index);

	private:
		uint32_t GenerateIndex();

		size_t m_nextIndex = 0;
		std::set<size_t> m_freeIndexes;

		SSBO m_ssbo;
	};

	class MultiIndexLocation
	{
	public:
		uint32_t Create(uint64_t base);
		void Destroy(uint64_t base, uint32_t index);

	private:
		std::map<uint64_t, IndexLocation> m_buffers;
	};
}