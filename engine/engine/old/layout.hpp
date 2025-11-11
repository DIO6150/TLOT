#pragma once

#include <string>
#include <vector>

namespace Engine
{
	struct LayoutElement {
		unsigned int elem_type;
		int elem_count;
	};


	class Layout {
	private:
		int m_stride;
		std::vector<LayoutElement> m_elements;

	public:
		Layout ();
		Layout (std::vector<LayoutElement> elements);

		void Apply ();
	};
}