#include <engine/data/Texture.hpp>

#include <engine/utils/Utils.hpp>

using namespace Engine::Data;
using namespace Engine::Core;

bool TextureQuad::operator== (TextureQuad & other) {
	return (x == other.x && y == other.y && w == other.w && h == other.h);
}

TextureQuad::TextureQuad (float x, float y, float w, float h, int depth):
	x {x}, y{y}, w{w}, h{h}, depth{depth}
	{

}

TextureQuad::TextureQuad ():
	x{0.0}, y{0.0}, w{0.0}, h{0.0}, depth{0}
	{

}

Texture::Texture () : 
	data {nullptr} {

}

Texture::Texture (unsigned char * data, size_t width, size_t height):
	data	{data},
	width	{width},
	height	{height}
	{

}

Texture::~Texture () {
	printf ("destruction de donnée de texture %p\n", data);
	stbi_image_free (data);
}

Texture & Texture::operator= (Texture && other) {
	if (data) {
		stbi_image_free (data);
	}

	data	= other.data;
	width	= other.width;
	height	= other.height;

	other.data = nullptr;

	return (*this);
}


TextureAtlas::TextureAtlas ():
	mHandle {0},
	mWidth {4096},
	mHeight {4096},
	mToUpdate {true}
	{

	TextureQuad _canvas;
	_canvas.x = 0;
	_canvas.y = 0;
	_canvas.w = mWidth;
	_canvas.h = mHeight;
	_canvas.depth = 0;

	mPartitions.push_back ({_canvas, false});
}

TextureAtlas::~TextureAtlas () {
	if (mHandle) {
		glDeleteTextures (1, &mHandle);
	}
}

unsigned int TextureAtlas::Get () const {
	return (mHandle);
}

static bool comp_quad_pair (std::pair<TextureQuad, bool> a, std::pair<TextureQuad, bool> b) {
	return (a.first.w * a.first.h < b.first.w * b.first.h);
}

// TODO-fix: add depth
// TODO-critical: if it can't find a partition large enough, it may break things, fix this asap
bool TextureAtlas::Feed (const Vector<HandleID> & textures) {
	bool _result = false;
	
	for (HandleID _textureID : textures) {
		if (mAtlas.find (_textureID) != mAtlas.end ()) {
			continue;
		}
		
		AssetManager * am = AssetManager::GetInstance ();
		
		const Texture * _texture = am->GetTexture (_textureID);
		size_t _width  = _texture->width;
		size_t _height = _texture->height;

		bool _generate_right = false;
		bool _generate_top = false;

		TextureQuad _right;
		TextureQuad _top;

		// the first partion we find will have the least amount of divided space
		std::sort (mPartitions.begin (), mPartitions.end (), comp_quad_pair);

		// we are trying to find a partition fitting the dimension of the texture in the partitioned space of the atlas
		for (auto &[_quad, _assigned] : mPartitions) {
			// partition already holds a texture
			if (_assigned) continue;

			// texture too big for parition
			if (_quad.w < _width || _quad.h < _height) continue;

			size_t _deltaWidth  = _quad.w - _width;
			size_t _deltaHeight = _quad.h - _height;

			// new partitions are created to fill in remaining space

			if (_quad.x + _width < mWidth) {
				_right.x = _quad.x + _width;
				_right.y = _quad.y;
				_right.w = _deltaWidth;
				_right.h = _height;
				_right.depth = _quad.depth;

				_generate_right = true;
			}

			if (_quad.y + _height < mHeight) {	
				_top.x = _quad.x;
				_top.y = _quad.y + _height;
				_top.w = _quad.w;
				_top.h = _deltaHeight;
				_top.depth = _quad.depth;

				_generate_top = true;
			}

			// partition is resized to fit texture
			_quad.w = _width;
			_quad.h = _height;
			_assigned = true;

			mAtlas.emplace (_textureID, std::make_unique<TextureQuad> (_quad));
			mToUpdate = true;
			_result = true;
			//mAtlas[_textureID] = std::make_unique<TextureQuad> (_quad);
			std::cout << "Added TextureID=" << _textureID << "\n";
			break;
		}

		if (_generate_right) {
			mPartitions.push_back ({_right, false});
		}
		if (_generate_top) {
			mPartitions.push_back ({_top,   false});
		}
	}

	return (_result);
}

void TextureAtlas::Generate () {
	if (mHandle) {
		glDeleteTextures (1, &mHandle);
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &mHandle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mHandle);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, mWidth, mHeight, 1);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//int i = 0;
	AssetManager * am = AssetManager::GetInstance ();

	for (const auto & [_textureID, _quad] : mAtlas) {
		const Texture * _texture = am->GetTexture (_textureID);

		//printf ("%f %f %f %f %d : %p\n", _quad->x, _quad->y, _quad->w, _quad->h, _quad->depth, _texture.data);

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			
			_quad->x, _quad->y, 0,
			_quad->w, _quad->h, 1,
			
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			_texture->data
		);
		//++i;
		//printf ("%d/%lld\n", i, mAtlas.size ());
	}

	mToUpdate = false;
}

const Engine::Data::TextureQuad * Engine::Data::TextureAtlas::Quad (HandleID texture) const {
	auto pos = mAtlas.find (texture);

	if (pos == mAtlas.end ()) {
		std::cout << "HandleID not found: " << texture << "\n";
		return (nullptr);
	}

	return (pos->second.get ());
}

size_t TextureAtlas::Width () const {
	return (mWidth);
}

size_t TextureAtlas::Height () const {
	return (mHeight);
}

void TextureAtlas::Resize (size_t newWidth, size_t newHeight) {
	mWidth  = newWidth;
	mHeight = newHeight;

	Reconstruct ();
	Generate ();
}

void TextureAtlas::Reconstruct () {
	Vector<HandleID> _textures;

	for (auto & [_id, _] : mAtlas) {
		_textures.push_back (_id);
	}

	mPartitions.clear ();
	mAtlas.clear ();

	TextureQuad _canvas;
	_canvas.x = 0;
	_canvas.y = 0;
	_canvas.w = mWidth;
	_canvas.h = mHeight;
	_canvas.depth = 1;

	mPartitions.push_back ({_canvas, false});

	Feed (_textures);
}