#pragma once

#include <stdint.h>

#include <TetoEngine/Common/VertexLayout.hpp>

#include <glad/glad.h>


namespace TetoEngine
{
	class VAO
	{
		public:
		void create();
		void destroy();
		
		void bind();
		void unbind();
		
		private:
		uint32_t object_;
	};
}
