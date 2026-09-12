#pragma once

#include <map>
#include <memory>
#include <unordered_map>

#include <Core/Resource.hpp>
#include <Internal/Glyph.hpp>
#include <OpenGL/TextureAtlas/TextureQuad.hpp>


namespace TLOT
{
	class GlyphAtlas
	{
	public:
		GlyphAtlas(size_t width, size_t height) : m_width(width), m_height(height) {}

		void InsertGlyph(char character, Glyph glyph, uint32_t fontSize, ResourceHandle font);
		TextureQuad GetGlyph(char character, uint32_t fontSize, ResourceHandle font);
		void Generate();
		unsigned int Get() { return m_handle; }


	private:
		uint64_t GenerateCharacterHash(char character, uint32_t fontSize, ResourceHandle font);

		unsigned int m_handle = 0;
		size_t m_width;
		size_t m_height;

		std::map<uint64_t, TextureQuad> m_quads;

		struct PendingGlyph
		{
			char character;
			uint32_t fontSize;
			Glyph data;
			ResourceHandle font;
		};

		std::vector<PendingGlyph> m_pendingGlyphs;
		bool m_shouldGenerate = true;
		
	};
}