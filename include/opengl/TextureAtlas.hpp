#pragma once

#include <map>
#include <memory>
#include <unordered_map>

#include <resources/Resource.hpp>
#include <core/TextureQuad.hpp>
#include <resources/Texture.hpp>

namespace TLOT
{
	// TODO-fix: assetManager should not be here, it should take raw texture data
	// and save them in the class itself
	class TextureAtlas
	{
	public:
	
		void Generate ();
		
		bool Feed (ResourceHandle handle, Texture const & texture);
		void Resize (size_t newWidth, size_t newHeight);
		
		TextureQuad Quad (ResourceHandle handle) const;
		
		size_t Width  () const;
		size_t Height () const;
		
		unsigned int Get () const;

		TextureAtlas ();
		~TextureAtlas ();

		TextureAtlas (TextureAtlas &  other) = default;
		TextureAtlas (TextureAtlas && other) = default;
		TextureAtlas (const TextureAtlas &  other) = default;
	private:
		void Reconstruct ();

		unsigned int m_handle = 0;
		size_t m_width  = 4096;
		size_t m_height = 4096;

		bool m_dirty = false;

		std::vector<std::pair<TextureQuad, bool>> m_partitions; // Quad, Is Assigned ?
		std::unordered_map<ResourceHandle, std::unique_ptr<TextureQuad>> m_quads;
		std::map<ResourceHandle, Texture> m_textures;
	};
}