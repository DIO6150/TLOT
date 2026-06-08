#pragma once

#include <vector>
#include <memory>

namespace TLOT
{
	struct VertexAtom
	{
		enum Type
		{
			Float, Vec2, Vec3, Vec4
		};

		VertexAtom(Type _type): value{_type}
		{

		}

		Type value;

		size_t Size ()
		{
			switch (value)
			{
				case Float: return   4ULL;
				case Vec2:  return   8ULL;
				case Vec3:  return  12ULL;
				case Vec4:  return  16ULL;
				default:    return   0ULL;
			}
		}
	};

	struct VertexLayout
	{
		VertexAtom atom;
		size_t size;
		size_t offset;

		bool operator==(VertexLayout & rhs) const
		{
			if (atom.value != rhs.atom.value) return false;
			if (size != rhs.size) return false;
			if (offset != rhs.offset) return false;

			return true;
		}
	};

	class VertexTemplate
	{
	public:
		VertexTemplate(std::vector<VertexAtom::Type> layout)
		{
			size_t offset = 0;
			for (auto _type : layout)
			{
				VertexAtom atom {_type};
				size_t size = atom.Size();
				m_layout.emplace_back (std::move(_type), size, offset);

				offset += size;
				m_size += size;
			}
		}

		size_t const Size () const { return m_size; }

		// could be optimized by comparing a hash
		bool operator==(VertexTemplate & rhs) const
		{
			if (m_size != rhs.m_size) return false;
			for(size_t index = 0; index < m_size; ++index)
			{
				if (m_layout.at(index) != rhs.m_layout.at(index))
					return false;
			}

			return true;
		}

		const std::vector<VertexLayout>& GetLayout() const { return m_layout; }

	private:
		std::vector<VertexLayout> m_layout;
		size_t m_size = 0;
	};

	struct Geometry
	{
		std::vector<float>  vertices;
		std::vector<uint32_t>  indices;
		
		std::shared_ptr<VertexTemplate> vertexTemplate;
	};
}
