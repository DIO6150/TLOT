#pragma once

#include <vector>

#include <Core/ID.hpp>

namespace TLOT
{
	// généré par gemini, j'y connais rien en fonction robuste de hashing
	// TODO: rework this, and if its good keep it idk
	// Fonction utilitaire pour combiner les hashs
	inline ID64_t hash_combine(ID64_t seed, ID64_t value)
	{
		// On utilise une constante (Golden Ratio) pour disperser les bits
		// Le décalage et le XOR permettent de mélanger la valeur actuelle avec la précédente
		return seed ^ (value + 0x9e3779b97f4a7c15LL + (seed << 6) + (seed >> 2));
	}
	
	inline ID64_t GenerateOrderedSetHash(const std::vector<ID64_t>& handles)
	{
		ID64_t hash = 0;
		
		for (ID64_t handle : handles)
		{
			hash = hash_combine(hash, handle);
		}
		
		return hash;
	}
}
	