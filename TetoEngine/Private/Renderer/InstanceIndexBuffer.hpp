#pragma once

#include <map>
#include <set>

#include <TetoEngine/Common/ID.hpp>


namespace TetoEngine
{
	class InstanceIndexBuffer
	{
	public:

		InstanceIndexBuffer();
		uint32_t get();
		void destroy(uint32_t index);

	private:
		uint32_t generateIndex();

		size_t nextIndex_ = 0;
		std::set<size_t> freeIndices_;
	};


	class MultiInstanceIndexBuffer
	{
	public:

		uint32_t get(ID64 base);
		void destroy(ID64 base, uint32_t index);

	private:

		std::map<ID64, InstanceIndexBuffer> buffers_;
	};
}