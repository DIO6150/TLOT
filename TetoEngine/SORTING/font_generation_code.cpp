
#include <ft2build.h>
#include FT_FREETYPE_H

#include <TetoEngine/Resource/Font.hpp>
#include <TetoEngine/Private/Renderer/Glyph.hpp>

#include <TetoEngine/Utils/Logger.hpp>

using namespace TetoEngine;

Font::Font()
{

}

void Font::GenerateSize(uint32_t size)
{
	if (glyphs_.find (size) != glyphs_.end()) return;


	FT_Set_Pixel_Sizes(face_, 0, size);

	for (unsigned char c = 0; c < 128; c++)
	{
		
		if (FT_Load_Char(face_, c, FT_LOAD_RENDER))
		{
			Logger::log(LogLevel::Error, "Can't load character {} for size {}", c, size);
			continue;
		}

		FT_Bitmap bitmap = face_->glyph->bitmap_;

		Glyph glyph;
		glyph.width_    = bitmap.width;
		glyph.height   = bitmap.rows;
		glyph.bearingX_ = face_->glyph->bitmap_left;
		glyph.bearingY_ = face_->glyph->bitmap_top;
		glyph.advance_  = face_->glyph->advance.x;

		if (bitmap.buffer)
		{
			glyph.bitmap_.assign(bitmap.buffer, bitmap.buffer + (bitmap.width * bitmap.rows));
		}

		glyphs_[size].emplace(c, glyph);
	}

}

std::map<char, Glyph> const & Font::GetGlyphs(uint32_t size)
{
	if (!glyphs_.contains(size)) GenerateSize(size);

	return glyphs_.at(size);
}

Glyph const & Font::getGlyph(char c, uint32_t size)
{
	return GetGlyphs(size).at(c);
}
