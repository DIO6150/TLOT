#pragma once

#include <vector>

#include <glad/glad.h>

namespace TetoEngine
{
	template<GLenum BufferType>
	class Buffer
	{
	public:
		Buffer()
		{
			glGenBuffers(1, &handle_);
		}

		void create(GLenum usage, size_t size)
		{
			usage_ = usage;
			size_  = size;
			
			glBindBuffer(BufferType, handle_);
			glNamedBufferData(handle_, size_, NULL, usage_);
		}

		template<class T>
		void upload(std::vector<T> const & data, size_t byteOffset)
		{
			glNamedBufferSubData(handle_, byteOffset, data.size () * sizeof (T), (void *) data.data ());
		}

		template<class T>
		void upload(T const & data, size_t byteOffset)
		{
			glNamedBufferSubData(handle_, byteOffset, sizeof (T), (void *) data.data ());
		}

		void bind()
		{
			glBindBuffer(BufferType, handle_);
		}

		void unbind()
		{
			glBindBuffer(BufferType, 0);
		}

	private:
		GLenum usage_;
		size_t size_ = 0;	
		uint32_t handle_;
	};
}
