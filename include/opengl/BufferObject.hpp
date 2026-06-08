#pragma once

#include <vector>

#include <glad/glad.h>


namespace TLOT
{
	template<GLenum BufferType>
	class BufferObject
	{
	public:
		BufferObject()
		{
			glGenBuffers(1, &m_handle);
		}

		void Create(GLenum usage, size_t size)
		{
			m_usage = usage;
			m_size  = size;
			
			glBindBuffer(BufferType, m_handle);
			glNamedBufferData(m_handle, size, NULL, m_usage);
		}

		template<class T>
		void Upload(std::vector<T> const & data, size_t byteOffset)
		{
			glNamedBufferSubData(m_handle, byteOffset, data.size () * sizeof (T), (void *) data.data ());
		}

		template<class T>
		void Upload(T const & data, size_t byteOffset)
		{
			glNamedBufferSubData(m_handle, byteOffset, sizeof (T), (void *) data.data ());
		}

		void Bind ()
		{
			glBindBuffer(BufferType, m_handle);
		}

		void Unbind ()
		{
			glBindBuffer(BufferType, 0);
		}

	private:
		GLenum m_usage;

		size_t m_size = 0;
		size_t m_capacity = 0;
		
		uint32_t m_handle;
	};
}
