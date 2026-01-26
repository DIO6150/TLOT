#include <engine/texture.hpp>
#include <engine/utils.hpp>


Engine::Texture::Texture () : data {nullptr}, width {0}, height {0}, type {Engine::TexType::NONE}, quad {nullptr}, key {} {

}

Engine::Texture::Texture (const std::string & name, const std::string & path, TexType type) : 
	type {type},
	quad {nullptr},
	key {name} {
	load_texture (path, data, width, height);
}

Engine::Texture::~Texture () {
	stbi_image_free (data);
}

Engine::TextureAtlas::TextureAtlas () :
	m_width {0},
	m_height	{0},
	m_cumul_width	{0},
	m_cumul_height	{0},
	m_cumul_depth 	{0},
	m_max_height 	{0},
	m_max_size	{4096 * 2},
	m_max_depth	{1}
{

}

void Engine::TextureAtlas::build (unsigned int n) {
	glActiveTexture(GL_TEXTURE0 + n);
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, m_max_size, m_max_size, m_max_depth);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	// Missing Texture

	m_missing_texture = (TextureQuad) { this, 0, 0, 2, 2, 0 };

	/*
	unsigned char data[16] = {
		207, 8, 174, 255,      0,   0,   0, 255,
		0,   0,   0, 255,    207,   8, 174, 255
	};
	*/

	unsigned char data[16] = {
		255, 255, 255, 255,    255, 255, 255, 255,
		255, 255, 255, 255,    255, 255, 255, 255
	};

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		0, 0, 0,
		2, 2, 1,
		GL_RGBA, GL_UNSIGNED_BYTE,
		data
	);

	m_cumul_width = 2;
	m_cumul_height = 2;
	m_max_height = 2;
}

Engine::TextureQuad * Engine::TextureAtlas::getQuad (const Texture * texture) {
	if (!texture || texture->key.empty ()) {
		return (&m_missing_texture);
	}

	auto pos = m_atlas.find (texture->key);
	if (pos != m_atlas.end()) return (pos->second.get ());

	if (m_max_height < texture->height) {
		m_max_height = texture->height;
	}

	if (m_cumul_width + texture->width > m_max_size) {
		m_cumul_height += texture->height;
		m_cumul_width = 0;
		m_max_height = 0;
	}

	if (m_cumul_height + texture->height > m_max_size) {
		printf ("ERROR: Too many textures in atlas.\n");
		return (nullptr);
	}

	m_atlas[texture->key] = std::make_unique<TextureQuad> (this, (float)m_cumul_width, (float)m_cumul_height, (float)texture->width, (float)texture->height, 0);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		m_cumul_width,  m_cumul_height,  0,
		texture->width, texture->height, 1,
		GL_RGBA, GL_UNSIGNED_BYTE,
		texture->data
	);

	m_cumul_width += texture->width;

	return (m_atlas [texture->key].get ());
}