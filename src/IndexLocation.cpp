#include <vector>

#include <Renderer/IndexLocation.hpp>
#include <iostream>

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
	//std::cout << "[FREE ] " << index << '\n';
	assert(!m_freeIndexes.contains(index));

	if (index > m_nextIndex)
	{
		std::cout << "[FREE ] WHAT ? \n";
	}

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

    //std::cout << "[ALLOC] " << index << '\n';

	if (m_nextIndex > 80 && index < 50)
	{
		std::cout << "[ALOC ] WHAT ? \n";
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
