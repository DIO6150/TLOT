#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <stdint.h>

namespace Engine {
	enum class TexType {
		NONE,
		DIFFUSE,
		SPECULAR
	};
	
	class TextureAtlas;

	struct TextureQuad {
		TextureAtlas * atlas;
		float x, y, w, h;
		int depth;
	};

	struct Texture {
		Texture ();
		Texture (const std::string & name, const std::string & path, TexType type);
		~Texture ();

		unsigned char * data;
		int		width, height;
		TexType		type;

		TextureQuad *	quad;

		std::string	key;
	};

	class TextureAtlas {
	public:
		TextureAtlas ();
		void build (unsigned int n);

		TextureQuad* getQuad (const Texture * texture);

		int getMaxSize () { return (m_max_size); }

		unsigned int texture_handle;

	private:
		std::unordered_map<std::string, std::unique_ptr<TextureQuad>> m_atlas;

		TextureQuad m_missing_texture;

		int m_width;
		int m_height;

		int m_cumul_width;
		int m_cumul_height;
		int m_cumul_depth;

		int m_max_height;

		int m_max_size;
		int m_max_depth;
	};
}