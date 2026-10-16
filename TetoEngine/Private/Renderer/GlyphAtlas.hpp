#pragma once

#include <map>
#include <memory>
#include <unordered_map>

#include <TetoEngine/Common/ID.hpp>

#include <TetoEngine/Private/Renderer/Glyph.hpp>
#include <TetoEngine/Private/Renderer/TextureQuad.hpp>

namespace TetoEngine
{
	class GlyphAtlas
	{
	public:
		GlyphAtlas(size_t width, size_t height) : width_(width), height_(height) {}

		void insertGlyph(char character, Glyph glyph, uint32_t fontSize, ID64 font);
		TextureQuad getGlyph(char character, uint32_t fontSize, ID64 font);
		void generate();
		unsigned int get() { return handle_; }


	private:
		ID64 generateCharacterHash(char character, uint32_t fontSize, ID64 font);

		unsigned int handle_ = 0;
		size_t width_;
		size_t height_;

		std::map<ID64, TextureQuad> quads_;

		struct PendingGlyph
		{
			char character_;
			uint32_t fontSize_;
			Glyph data_;
			ID64 font_;
		};

		std::vector<PendingGlyph> pendingGlyphs_;
		bool dirty_ = true;
		
	};
}