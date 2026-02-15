#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <stdint.h>

#include <engine/core/AssetManager.hpp>

#include <engine/utils/Types.hpp>

namespace Engine::Data {
	class TextureAtlas;

	struct TextureQuad {
		TextureQuad ();
		TextureQuad (float x, float y, float w, float h, int depth);
		
		bool operator== (TextureQuad & other);

		float x, y, w, h;
		int depth;

		TextureQuad (TextureQuad & other)  = default;
		TextureQuad (TextureQuad && other) = default;

		TextureQuad & operator= (const TextureQuad & other) = default;

		TextureQuad & operator= (TextureQuad && other) = default;
		TextureQuad & operator= (TextureQuad & other) = default;
	};

	struct Texture {
		Texture ();
		Texture (unsigned char * data, size_t width, size_t height);
		~Texture ();

		Texture & operator= (Texture && other);

		unsigned char * data;
		size_t		width;
		size_t		height;

		Texture (Texture &  other) = delete;
		Texture (Texture && other) = delete;
		Texture (const Texture &  other) = delete;
		Texture (const Texture && other) = delete;

		Texture & operator= (Texture & other) = delete;
		Texture & operator= (const Texture & other) = delete;
		Texture & operator= (const Texture && other) = delete;
	};

	class TextureAtlas {
	public:
		TextureAtlas (const Core::AssetManager * assetManager);
		~TextureAtlas ();

		TextureAtlas (TextureAtlas &  other) = default;
		TextureAtlas (TextureAtlas && other) = default;
		TextureAtlas (const TextureAtlas &  other) = default;

		unsigned int Get () const;

		void Feed (std::vector<Core::TextureID> textures);
		
		void Generate ();
		
		const TextureQuad * Quad (Core::TextureID texture) const;
		size_t Width  () const;
		size_t Height () const;

		void Resize (size_t newWidth, size_t newHeight);
		
	private:
		void Reconstruct ();

		unsigned int mHandle;
		size_t mWidth;
		size_t mHeight;

		std::vector<std::pair<TextureQuad, bool>> mPartitions; // Quad, Assigned
		std::unordered_map<Core::TextureID, std::unique_ptr<TextureQuad>> mAtlas;
		
		const Core::AssetManager * pAssetManager;
	};
}