#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <Internal/Glyph.hpp>

#include <iostream>

namespace TLOT
{
	class Font
	{
	public:
		Font() : m_ft(nullptr), m_face(nullptr) {}

		~Font()
		{
			//if (m_face) FT_Done_Face(m_face);
			//if (m_ft)   FT_Done_FreeType(m_ft);
		}

		void GenerateSize(uint32_t size)
		{
			if (m_glyphs.find (size) != m_glyphs.end())
				return;

			FT_Set_Pixel_Sizes(m_face, 0, size);

			for (unsigned char c = 0; c < 128; c++)
			{
				
				if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
				{
					std::cerr << "Erreur : Impossible de charger le caractère '" << c << "'" << std::endl;
					continue;
				}

				FT_Bitmap bitmap = m_face->glyph->bitmap;

				Glyph glyph;
				glyph.width    = bitmap.width;
				glyph.height   = bitmap.rows;
				glyph.bearingX = m_face->glyph->bitmap_left;
				glyph.bearingY = m_face->glyph->bitmap_top;
				glyph.advance  = m_face->glyph->advance.x;

				if (c == 'h')
				{
					std::cout
					<< "Taille de police: " << size
					<< " Lettre: " << c 
					<< " | W: " << glyph.width 
					<< " | H: " << glyph.height 
					<< " | BearingX: " << glyph.bearingX 
					<< " | BearingY: " << glyph.bearingY 
					<< " | Advance: " << glyph.advance << std::endl;
				}

				if (bitmap.buffer) {
					glyph.bitmap.assign(bitmap.buffer, bitmap.buffer + (bitmap.width * bitmap.rows));
				}

				m_glyphs[size].emplace(c, glyph);
			}

		}

		const Glyph& getGlyph(char c, uint32_t size)
		{
			if (m_glyphs.find(size) == m_glyphs.end())
			{
				GenerateSize(size);
			}

			return m_glyphs.at(size).at(c);
		}
		
		const std::map<uint32_t, std::map<char, Glyph>>& getGlyphs() const
		{
			return m_glyphs;
		}

		FT_Library m_ft;
		FT_Face m_face;
		std::map<uint32_t, std::map<char, Glyph>> m_glyphs;

		friend class AssetManager;
	};
}