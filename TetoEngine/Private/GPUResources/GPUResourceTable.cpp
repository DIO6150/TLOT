
#include <TetoEngine/Private/GPUResources/GPUResourceTable.hpp>

#include <algorithm>

using namespace TetoEngine;

GPUResourceTable::GPUResourceTable(uint32_t capacity)
	: capacity_ {capacity}
{
	partitions_.emplace_back(Slot {.offset = 0, .size = capacity_});
}

std::optional<uint32_t> GPUResourceTable::add(ID64 handle, uint32_t size)
{
	if (size > capacity_)
		return std::nullopt;

	auto it = slots_.find(handle);

	if (it != slots_.end())
		return it->second.offset;

	for (auto index = 0; index < partitions_.size(); ++index)
	{
		Slot part = partitions_.at(index);

		if (part.size < size)
			continue;

		Slot slot {
			.offset=part.offset,
			.size=size
		};

		if (slot.size == part.size)
		{
			partitions_.erase(partitions_.begin() + index);
		}
		else
		{
			partitions_[index] = Slot {
				.offset = slot.offset + slot.size,
				.size   = part.size - slot.size
			};
		}

		slots_.emplace(handle, slot);
		size_ += slot.size;
		return slot.offset;
	}

	return std::nullopt;
}

std::optional<uint32_t> GPUResourceTable::byteOffset(ID64 handle) const
{
	auto it = slots_.find(handle);

	if (it == slots_.end())
		return std::nullopt;

	return it->second.offset;
}

void GPUResourceTable::remove(ID64 handle)
{
	auto it = slots_.find(handle);

	if (it == slots_.end())
		return;

	Slot slot = it->second;
	slots_.erase(handle);
	partitions_.push_back(slot);

	size_ -= slot.size;
	
	mergePartitions();
}

void GPUResourceTable::pack()
{
	std::vector<std::pair<ID64, uint32_t>> allocations;

	for (auto const & [handle, slot] : slots_)
		allocations.emplace_back(handle, slot.size);

	std::sort(
		allocations.begin(),
		allocations.end(),
		[](auto const & a, auto const & b)
		{
			return a.second < b.second;
		}
	);

	slots_.clear();
	partitions_.clear();

	uint32_t offset = 0;

	for (auto const & [handle, size] : allocations)
	{
		if (size > capacity_ - offset)
		{
			return; // should not be possible
		}

		slots_.emplace(
			handle,
			Slot {.offset = offset, .size = size}
		);
	}

	if (offset < capacity_)
	{
		partitions_.emplace_back(
			Slot { .offset = offset, .size = capacity_ - offset }
		);
	}
}

void GPUResourceTable::clear()
{
	slots_     .clear();
	partitions_.clear();
	partitions_.emplace_back(Slot {.offset = 0, .size = capacity_});
	size_ = 0;
}

size_t GPUResourceTable::size() const
{
	return size_;
}

size_t GPUResourceTable::capacity() const
{
	return capacity_;
}

size_t GPUResourceTable::remaining() const
{
	if (size_ > capacity_)
		return 0;

	return capacity_ - size_;
}

void GPUResourceTable::sortPartitions()
{
	std::sort(partitions_.begin(), partitions_.end(),
		[](auto const & a, auto const & b)
		{
			return a.offset < b.offset;
		}
	);
}

void GPUResourceTable::mergePartitions()
{
	if (partitions_.empty())
		return;

	sortPartitions();

	size_t index = 1;

	while (index < partitions_.size())
	{
		Slot & previous = partitions_.at(index - 1);
		Slot   current  = partitions_.at(index);

		auto previousEnd = previous.offset + previous.size;
		auto currentEnd  = current .offset + current .size;

		if (previousEnd >= current.offset)
		{
			auto mergedEnd = std::max(previousEnd, currentEnd);
			previous.size = mergedEnd - previous.offset;

			partitions_.erase(partitions_.begin() + index);
		}
		else
		{
			++index;
		}
	}
}
