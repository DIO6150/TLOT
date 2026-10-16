
#include <cassert>
#include <vector>
#include <iostream>

#include <TetoEngine/Private/Renderer/InstanceIndexBuffer.hpp>

using namespace TetoEngine;

InstanceIndexBuffer::InstanceIndexBuffer()
{

}

uint32_t InstanceIndexBuffer::get()
{
	return generateIndex();
}

void InstanceIndexBuffer::destroy(uint32_t index)
{
	assert(!freeIndices_.contains(index));

	freeIndices_.emplace(index);
}

uint32_t InstanceIndexBuffer::generateIndex()
{
	uint32_t index;

    if (freeIndices_.empty())
    {
        index = nextIndex_++;
    }
    else
    {
        index = *freeIndices_.begin();
        freeIndices_.erase(freeIndices_.begin());
    }

    return index;
}

uint32_t MultiInstanceIndexBuffer::get(ID64 base)
{
	buffers_.at(base).get();
}

void MultiInstanceIndexBuffer::destroy(ID64 base, uint32_t index)
{
	if (!buffers_.contains(base))
		return;

	buffers_.at(base).destroy(index);
}
