#pragma once

#include <Core/Logger.hpp>
#include <glad/glad.h>
#include <stdint.h>

#include <Geometry.hpp>


namespace TLOT
{
    class VAO
	{
	public:
		VAO ()
		{
			glGenVertexArrays(1, &m_object);
		}

		void Create(std::shared_ptr<VertexTemplate> vertexTemplate)
		{
			if (!vertexTemplate) return;

			GLsizei vertexSize = vertexTemplate->Size();
			//Logger::log(LogLevel::Info, "size = {}", vertexSize);
			
			const auto& layouts = vertexTemplate->GetLayout();
			GLuint attributeIndex = 0;

			Bind();
			for (const auto& layout : layouts)
			{
				GLint componentCount = 0;
				GLenum glType = GL_FLOAT;

				switch (layout.atom.value)
				{
					case TLOT::VertexAtom::Float: 
						componentCount = 1; 
						glType = GL_FLOAT; 
						break;
					case TLOT::VertexAtom::Vec2:  
						componentCount = 2; 
						glType = GL_FLOAT; 
						break;
					case TLOT::VertexAtom::Vec3:  
						componentCount = 3; 
						glType = GL_FLOAT; 
						break;
					case TLOT::VertexAtom::Vec4:  
						componentCount = 4; 
						glType = GL_FLOAT; 
						break;
				}

				glVertexAttribPointer(
					attributeIndex, 
					componentCount, 
					glType, 
					GL_FALSE,
					vertexSize,
					reinterpret_cast<void*>(layout.offset)
				);

				glEnableVertexAttribArray(attributeIndex);

				attributeIndex++;
			}
			Unbind();
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