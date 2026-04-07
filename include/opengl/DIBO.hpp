#pragma once

#include <glad/glad.h>
#include <stdint.h>
#include <vector>

#include <opengl/DrawCommand.hpp>

namespace TLOT
{
    class DIBO
	{
    public:
		DIBO (uint32_t commandCount, GLenum usage):
			m_commandCount {commandCount},
			m_usage {usage}
        {
			
		}

		void Create ()
		{
			glGenBuffers (1, &m_object);
			glBindBuffer (GL_DRAW_INDIRECT_BUFFER, m_object);
			glNamedBufferData (m_object, m_commandCount * sizeof (DrawCommand), NULL, m_usage);
		}
        
        // TODO-fix: index and size checking + resizing
		void Push (DrawCommand const & draw_command, size_t offset)
		{
			glNamedBufferSubData (m_object, offset * sizeof (DrawCommand), sizeof (DrawCommand), (void *) &draw_command);
		}
        
		void ReplaceAll (std::vector<DrawCommand> & commands)
		{
            glNamedBufferSubData (m_object, 0, sizeof (DrawCommand) * commands.size (), (void *) commands.data ());
		}
        
		void Bind ()
		{
            glBindBuffer (GL_DRAW_INDIRECT_BUFFER, m_object);
		}
        
		void Unbind ()
		{
            glBindBuffer (GL_DRAW_INDIRECT_BUFFER, 0);
		}

    private:
        uint32_t m_object;

		uint32_t m_commandCount;
		GLenum m_usage;
	};
}