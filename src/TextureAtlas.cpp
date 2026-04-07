
#include <algorithm>

#include <glad/glad.h>

#include <resources/Texture.hpp>

#include <opengl/TextureAtlas.hpp>

#include <modules/AssetManager.hpp>

#include <core/Logger.hpp>


using namespace TLOT;

TextureAtlas::TextureAtlas ()
{
	TextureQuad canvas;
	canvas.x = 0;
	canvas.y = 0;
	canvas.w = m_width;
	canvas.h = m_height;
	canvas.depth = 0;

	m_partitions.push_back ({canvas, false});
}

TextureAtlas::~TextureAtlas () {
	if (m_handle) {
		glDeleteTextures (1, &m_handle);
	}
}

unsigned int TextureAtlas::Get () const {
	return (m_handle);
}

static bool comp_quad_pair (std::pair<TextureQuad, bool> a, std::pair<TextureQuad, bool> b) {
	return (a.first.w * a.first.h < b.first.w * b.first.h);
}

// TODO-fix: add depth
// TODO-critical: if it can't find a partition large enough, it may break things, fix this asap
bool TextureAtlas::Feed (ResourceHandle handle, Texture const & texture) {
	if (m_quads.find (handle) != m_quads.end ()) {
		return false;
	}

	m_textures.emplace (handle, texture);

	size_t width  = texture.width;
	size_t height = texture.height;

	bool generate_right = false;
	bool generate_top = false;

	TextureQuad right;
	TextureQuad top;

	// the first partion we find will have the least amount of divided space
	std::sort (m_partitions.begin (), m_partitions.end (), comp_quad_pair);

	// we are trying to find a partition fitting the dimension of the texture in the partitioned space of the atlas
	for (auto & [quad, assigned] : m_partitions) {
		// partition already holds a texture
		if (assigned) continue;

		// texture too big for parition
		if (quad.w < width || quad.h < height) continue;

		size_t deltaWidth  = quad.w - width;
		size_t deltaHeight = quad.h - height;

		// new partitions are created to fill in remaining space

		if (quad.x  + width < m_width) {
			right.x = quad.x + width;
			right.y = quad.y;
			right.w = deltaWidth;
			right.h = height;
			right.depth = quad.depth;

			generate_right = true;
		}

		if (quad.y + height < m_height) {	
			top.x  = quad.x;
			top.y  = quad.y + height;
			top.w  = quad.w;
			top.h  = deltaHeight;
			top.depth = quad.depth;

			generate_top = true;
		}

		// partition is resized to fit texture
		quad.w = width;
		quad.h = height;
		assigned = true;

		m_quads.emplace (handle, std::make_unique<TextureQuad> (quad));
		m_dirty = true;
		break;
	}

	if (generate_right) m_partitions.push_back ({right, false});
	if (generate_top  ) m_partitions.push_back ({top,   false});

	return true;
}

void TextureAtlas::Generate () {
	if (!m_dirty) return;

	if (m_handle) {
		glDeleteTextures (1, &m_handle);
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_handle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, m_width, m_height, 1);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	for (const auto & [textureID, quad] : m_quads) {
		Texture const & texture = m_textures[textureID];
		
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			quad->x, quad->y, 0,
			quad->w, quad->h, 1,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			texture.data
		);
	}

	m_dirty = false;
}

TextureQuad TextureAtlas::Quad (ResourceHandle texture) const {
	auto pos = m_quads.find (texture);

	if (pos == m_quads.end ()) {
		Logger::panic ("Handle is not registered in this atlas");
		return TextureQuad {};
	}

	return *pos->second.get ();
}

size_t TextureAtlas::Width () const {
	return m_width;
}

size_t TextureAtlas::Height () const {
	return m_height;
}

void TextureAtlas::Resize (size_t newWidth, size_t newHeight) {
	m_width  = newWidth;
	m_height = newHeight;

	Reconstruct ();
}

void TextureAtlas::Reconstruct () {
	std::vector<ResourceHandle> textures;

	for (auto & [id, _] : m_quads) {
		textures.push_back (id);
	}

	m_partitions.clear ();
	m_quads.clear ();

	TextureQuad canvas;
	canvas.x = 0;
	canvas.y = 0;
	canvas.w = m_width;
	canvas.h = m_height;
	canvas.depth = 1;

	m_partitions.push_back ({canvas, false});

	for (auto const & textureID : textures)
	{
		Texture const & texture = m_textures[textureID];
		Feed (textureID, texture);
	}
}
