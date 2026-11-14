#pragma once

#include "glad/glad.h"

#include <stdint.h>
#include <vector>

namespace TetoEngine
{
    class ShaderStorageBuffer
	{
	public:
		void create(GLenum usage, uint32_t binding, size_t baseElementCount);
		void destroy();

		template<class T>
		void upload(std::vector<T> const & data, size_t byteOffset)
		{
			glNamedBufferSubData
			(
				object_,
				byteOffset,
				data.size() * sizeof(T),
				data.data()
			);
		}

		template<class T>
		void upload(T const & data, size_t byteOffset)
		{
			glNamedBufferSubData
			(
				object_,
				byteOffset,
				sizeof(T),
				&data
			);
		}

		void bind();
		void unbind();

	private:
		uint32_t object_;
		uint32_t binding_;
		GLenum   usage_;
	};
}