#pragma once

#include <glad/glad.h>

#include <stdint.h>
#include <vector>

namespace TLOT
{
    class SSBO
	{
	public:
		void Create(GLenum usage, uint32_t binding, size_t baseElementCount);
		void Destroy();

		template<class T>
		void Upload(std::vector<T> const & data, size_t byteOffset)
		{
			glNamedBufferSubData
			(
				m_object,
				byteOffset,
				data.size() * sizeof(T),
				data.data()
			);
		}

		template<class T>
		void Upload(T const & data, size_t byteOffset)
		{
			glNamedBufferSubData
			(
				m_object,
				byteOffset,
				sizeof(T),
				&data
			);
		}

		void Bind();
		void Unbind();

	private:
		uint32_t m_object;
		uint32_t m_binding;
		GLenum   m_usage;
	};
}