
#include <TetoEngine/Common/ID.hpp>

using namespace TetoEngine;

ID64 TetoEngine::hash_combine(ID64 seed, ID64 value)
{
	return seed ^ (value + 0x9e3779b97f4a7c15LL + (seed << 6) + (seed >> 2)); // magic idk
}

ID64 TetoEngine::generate_ordered_set_hash(std::vector<ID64> const & handles)
{
	ID64 hash = 0;
	
	for (ID64 handle : handles)
	{
		hash = hash_combine(hash, handle);
	}
	
	return hash;
}
