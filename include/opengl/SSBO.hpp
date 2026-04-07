#pragma once

#include <glad/glad.h>

#include <stdint.h>
#include <vector>

namespace TLOT
{

	template<class Data>
    class SSBO
	{
	public:
		SSBO (size_t element_count, GLenum usage, uint32_t binding):
			m_elementCount {element_count},
			m_usage {usage},
			m_binding {binding}
		{

		}

		~SSBO ()
		{
			glDeleteBuffers (1, &m_object);
		}
		
		void Create ()
		{
			glGenBuffers (1, &m_object);
			glBindBuffer (GL_SHADER_STORAGE_BUFFER, m_object);
			glBindBufferBase (GL_SHADER_STORAGE_BUFFER, m_binding, m_object);
			glNamedBufferData (m_object, m_elementCount * sizeof (Data), NULL, m_usage);
		}

		void Push(Data const & data, size_t offset)
		{
			glBindBufferBase (GL_SHADER_STORAGE_BUFFER, m_binding, m_object);

			Logger::log (LogLevel::Info, "Pushing Data sizeof={} at offset={} (total={}) at binding={}", sizeof (Data), offset, sizeof (Data) * offset, m_binding);

			glNamedBufferSubData
			(
				m_object,
				sizeof (Data) * offset,
				sizeof (Data),
				(void*) &data
			);
		}

		void ReplaceAll (std::vector<Data> const & data_storage)
		{
			glBindBufferBase (GL_SHADER_STORAGE_BUFFER, m_binding, m_object);

			glNamedBufferSubData
			(
				m_object,
				0,
				sizeof (Data) * data_storage.size (),
				(void*) data_storage.data ()
			);
		}

		void Bind ()
		{
			glBindBufferBase (GL_SHADER_STORAGE_BUFFER, m_binding, m_object);
		}

		void Unbind ()
		{
			glBindBufferBase (GL_SHADER_STORAGE_BUFFER, m_binding, 0);
		}

	private:
		uint32_t m_object;

		size_t m_elementCount;
		GLenum m_usage;
		uint32_t m_binding;
	};
}