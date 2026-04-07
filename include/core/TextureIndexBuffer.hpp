#pragma once

#include <map>
#include <vector>

#include <resources/Resource.hpp>

namespace TLOT
{
	// TODO: should be able to "compress" list of textures
	// i.e. Insert ({0, 1, 2}) and Insert ({0, 1, 2}) should return the same index << solved
	// Insert ({1, 2}) could return the index+1 but idk seems kinda hard (or maybe xor magic can save me ???)
	// Maybe add a "ref counter" that allow space reusing when deleting instances ?? idk
	class TextureIndexBuffer
	{
	public:
		uint32_t GetIndex (std::vector<ResourceHandle> textures)
		{
			Hash hash = GenerateOrderedSetHash (textures);

			if (m_texturesIndices.find (hash) != m_texturesIndices.end ())
			{
				return m_texturesIndices[hash];
			}

			uint32_t index = m_nextIndex;

			m_texturesIndices[hash] = index;

			m_nextIndex += textures.size ();

			return index;
		}

		Hash GetHash (std::vector<ResourceHandle> textures)
		{
			return GenerateOrderedSetHash (textures);
		}

	private:
		std::map<Hash, size_t> m_texturesIndices;
		uint32_t m_nextIndex = 0;
	};
}