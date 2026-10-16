#include <TetoEngine/Common/VertexLayout.hpp>

using namespace TetoEngine;

VertexType::VertexType(Type type)
: type_{type}
{
	
}

size_t VertexType::size()
{
	switch (type_)
	{
		case Float: return   4ULL;
		case Vec2:  return   8ULL;
		case Vec3:  return  12ULL;
		case Vec4:  return  16ULL;
		default:    return   0ULL;
	}
}

VertexLayout::VertexLayout(std::vector<VertexType::Type> layout)
{
	size_t offset = 0;

	for (auto type : layout)
	{
		VertexType vertexType {type};
		auto size = vertexType.size();

		layout_.emplace_back (vertexType, size_, offset);

		offset += size;
		size_ += size;
	}
}

size_t const VertexLayout::size() const
{
	return size_;
}

bool VertexLayout::operator==(VertexLayout & rhs) const
{
	// could be optimized by comparing a hash instead
	if (size_ != rhs.size_) return false;

	for(size_t index = 0; index < size_; ++index)
	{
		if (layout_.at(index) != rhs.layout_.at(index))
			return false;
	}

	return true;
}

bool VertexLayout::VertexInfo::operator==(VertexLayout::VertexInfo & rhs) const
{
	if (typeInfo_.type_ != rhs.typeInfo_.type_) return false;
	if (size_       != rhs.size_)       return false;
	if (offset_     != rhs.offset_)     return false;

	return true;
}