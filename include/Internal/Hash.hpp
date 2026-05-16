#pragma once

#include <vector>

#include <Core/ID.hpp>

namespace TLOT
{
	// généré par gemini, j'y connais rien en fonction robuste de hashing
	// TODO: rework this, and if its good keep it idk
	// Fonction utilitaire pour combiner les hashs
	inline ID_64 hash_combine(ID_64 seed, ID_64 value)
	{
		// On utilise une constante (Golden Ratio) pour disperser les bits
		// Le décalage et le XOR permettent de mélanger la valeur actuelle avec la précédente
		return seed ^ (value + 0x9e3779b97f4a7c15LL + (seed << 6) + (seed >> 2));
	}
	
	static inline ID_64 GenerateOrderedSetHash(const std::vector<ID_64>& handles)
	{
		ID_64 hash = 0;
		
		for (ID_64 handle : handles)
		{
			hash = hash_combine(hash, handle);
		}
		
		return hash;
	}
}
	