#include <vector>

#include <Renderer/IndexLocation.hpp>
#include <iostream>

using namespace TLOT;

IndexLocation::IndexLocation()
{

}

uint32_t IndexLocation::Create()
{
	return GenerateIndex();
}

void IndexLocation::Destroy(uint32_t index)
{
	assert(!m_freeIndexes.contains(index));

	m_freeIndexes.emplace(index);
}

uint32_t IndexLocation::GenerateIndex()
{
	//return m_nextIndex++; // quand j'utilises pas le système de réutilisation d'indice, ya pas de soucis

	uint32_t index;

    if (m_freeIndexes.empty())
    {
        index = m_nextIndex++;
    }
    else
    {
        index = *m_freeIndexes.begin();
        m_freeIndexes.erase(m_freeIndexes.begin());
    }

    return index;
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
