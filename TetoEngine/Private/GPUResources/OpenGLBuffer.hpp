#pragma once

#include <vector>

#include <glad/glad.h>

namespace TetoEngine
{
	class OpenGLBuffer
	{
	public:
		OpenGLBuffer(GLenum bufferType, GLenum usage, size_t size)
			: bufferType_{bufferType}
			, usage_{usage}
			, size_{size}
		{
			glGenBuffers(1, &handle_);

			glBindBuffer(bufferType_, handle_);
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
			glBindBuffer(bufferType_, handle_);
		}

		void unbind()
		{
			glBindBuffer(bufferType_, 0);
		}

	private:
		uint32_t handle_;
		
		GLenum bufferType_;
		GLenum usage_;
		size_t size_ = 0;
	};
}
