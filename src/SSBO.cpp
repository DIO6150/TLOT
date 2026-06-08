#include <OpenGL/SSBO.hpp>

using namespace TLOT;

void SSBO::Create(GLenum usage, uint32_t binding, size_t baseElementCount)
{
	m_usage   = usage;
	m_binding = binding;

	glGenBuffers(1, &m_object);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_object);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_binding, m_object);
	glNamedBufferData(m_object, baseElementCount * sizeof(uint8_t), NULL, m_usage);
}

void SSBO::Destroy()
{
	glDeleteBuffers (1, &m_object);
}

void SSBO::Bind()
{
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, m_binding, m_object);
}

void SSBO::Unbind()
{
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, m_binding, 0);
}
