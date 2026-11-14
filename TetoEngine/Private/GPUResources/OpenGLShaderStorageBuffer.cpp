#include <TetoEngine/Private/GPUResources/OpenGLShaderStorageBuffer.hpp>

using namespace TetoEngine;

void ShaderStorageBuffer::create(GLenum usage, uint32_t binding, size_t baseElementCount)
{
	usage_   = usage;
	binding_ = binding;

	glGenBuffers(1, &object_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, object_);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_, object_);
	glNamedBufferData(object_, baseElementCount * sizeof(uint8_t), NULL, usage_);
}

void ShaderStorageBuffer::destroy()
{
	glDeleteBuffers (1, &object_);
}

void ShaderStorageBuffer::bind()
{
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, binding_, object_);
}

void ShaderStorageBuffer::unbind()
{
	glBindBufferBase (GL_SHADER_STORAGE_BUFFER, binding_, 0);
}
