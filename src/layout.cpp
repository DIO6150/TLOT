#include "layout.hpp"

#include "glad/glad.h"
#include "utils.hpp"

Engine::Layout::Layout (std::vector<LayoutElement> elements) :  m_stride (0), m_elements (elements) {
	for (auto& e : elements) {
		m_stride += e.elem_count * getGLTypeSize (e.elem_type);
	}
}

void Engine::Layout::Apply () {
	int i;
	intptr_t offset;

	i 	= 0;
	offset 	= 0;

	for (auto& e : m_elements) {
		glVertexAttribPointer (
			i, 
			e.elem_count,
			e.elem_type,
			GL_FALSE,
			m_stride,
			(void *) offset);

		glEnableVertexAttribArray (i++);

		offset += e.elem_count * getGLTypeSize (e.elem_type);
	}
}