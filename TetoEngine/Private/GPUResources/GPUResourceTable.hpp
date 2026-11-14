#pragma once

#include <TetoEngine/Common/ID.hpp>

#include <map>
#include <optional>

namespace TetoEngine
{
	// only lookup resource location, no control over the gpu
	class GPUResourceTable
	{
	public:
		GPUResourceTable(uint32_t capacity);

		std::optional<uint32_t> add(ID64 handle, uint32_t size);
		std::optional<uint32_t> byteOffset(ID64 handle) const;
		void remove(ID64 handle);

		void pack();
		void clear();

		size_t size() const;
		size_t capacity() const;
		size_t remaining() const;

	private:
		void sortPartitions();
		void mergePartitions();

	private:
		struct Slot
		{
			uint32_t offset;
			uint32_t size;
		};

		uint32_t capacity_ = 0;
		uint32_t size_ = 0;

		std::map<ID64, Slot> slots_;
		std::vector<Slot> partitions_;
	};
}