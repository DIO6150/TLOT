
#include <TetoEngine/Private/Renderer/VAO.hpp>

using namespace TetoEngine;

void VAO::create()
{
	glGenVertexArrays(1, &object_);

	VertexLayout layout {
		{
			VertexType::Vec3,
			VertexType::Vec2,
			VertexType::Vec3,
		}
	};

	GLsizei vertexSize = layout.size();

	GLuint attributeIndex = 0;
	bind();
	for (const auto& vertexInfo : layout)
	{
		GLint componentCount = 0;
		GLenum glType = GL_FLOAT;

		switch (vertexInfo.typeInfo_.type_)
		{
			case TetoEngine::VertexType::Float: 
				componentCount = 1;
				glType = GL_FLOAT;
				break;

			case TetoEngine::VertexType::Vec2:  
				componentCount = 2;
				glType = GL_FLOAT;
				break;

			case TetoEngine::VertexType::Vec3:  
				componentCount = 3;
				glType = GL_FLOAT;
				break;

			case TetoEngine::VertexType::Vec4:  
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
			reinterpret_cast<void*>(vertexInfo.offset_)
		);

		glEnableVertexAttribArray(attributeIndex);

		attributeIndex++;
	}
	unbind();
}

void VAO::destroy()
{
	glDeleteVertexArrays (1, &object_);
}

void VAO::bind()
{
	glBindVertexArray (object_);
}

void VAO::unbind()
{
	glBindVertexArray (0);
}
