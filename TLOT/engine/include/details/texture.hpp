#pragma once

#include <string>
#include <vector>

#include <stdint.h>

namespace ED {
	// Its called Texture but its really a texture array
	class Texture {
	public:
		Texture ();

		/// @brief Load texture and place it into the OpenGL TextureAtlas
		/// @param url path to the texture
		/// @return layer in which the texture was placed
		uint32_t loadAtlas (const std::string & url);


	private:
		uint32_t	m_handle;

		uint32_t	m_width, m_height, m_depth, m_layer;

		std::vector<std::string> m_names;
	};
}