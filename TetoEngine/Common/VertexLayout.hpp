#pragma once

#include <vector>

namespace TetoEngine
{
	struct VertexType
	{
		enum Type { Float, Vec2, Vec3, Vec4 };

		Type type_;

		VertexType(Type type);
		size_t size();
	};


	class VertexLayout
	{
	public:

		VertexLayout() = default;
		VertexLayout(std::vector<VertexType::Type> layout);
		size_t const size () const;
		bool operator==(VertexLayout & rhs) const;

		auto begin() { return layout_.begin(); }
		auto end()   { return layout_.end  (); }

	private:

		struct VertexInfo
		{
			VertexType typeInfo_;
			size_t size_;
			size_t offset_;

			bool operator==(VertexInfo & rhs) const;
		};

		std::vector<VertexInfo> layout_;
		size_t size_ = 0;
	};
}
