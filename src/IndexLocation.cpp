#include <vector>

#include <Renderer/IndexLocation.hpp>


using namespace TLOT;

IndexLocation::IndexLocation()
{
	m_ssbo.Create(GL_DYNAMIC_DRAW, 2, 10000000); // 10Mo
}

uint32_t IndexLocation::Create()
{
	return GenerateIndex();
}

void IndexLocation::Destroy(uint32_t index)
{
	m_freeIndexes.emplace(index);
}

uint32_t IndexLocation::GenerateIndex()
{
	if(m_freeIndexes.empty())
	{
		return m_nextIndex++;
	}

	for (auto index : m_freeIndexes)
	{
		m_freeIndexes.erase(index);
		return index;
	}
}

uint32_t MultiIndexLocation::Create(uint64_t base)
{
	return m_buffers[base].Create();
}

void MultiIndexLocation::Destroy(uint64_t base, uint32_t index)
{
	if (m_buffers.find(base) == m_buffers.end())
		return;

	m_buffers[base].Destroy(index);
}
