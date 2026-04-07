#pragma once

#include <glad/glad.h>
#include <stdint.h>

namespace TLOT
{
    class VAO
	{
	public:
		VAO ()
		{
			glGenVertexArrays(1, &m_object);
		}

		void Bind ()
		{
			glBindVertexArray (m_object);
		}

		void Unbind ()
		{
			glBindVertexArray (0);
		}

		~VAO ()
		{
			glDeleteVertexArrays (1, &m_object);
		}

	private:
		uint32_t m_object;
	};
}