
#include <glad/glad.h>

#include <TetoEngine/Private/Renderer/TextureAtlas.hpp>
#include <TetoEngine/Application/Application.hpp>

#include <algorithm>

using namespace TetoEngine;

TextureAtlas::TextureAtlas(Application * app, uint32_t width, uint32_t height, uint32_t depth)
	: app_   {app}
	, width_ {width}
	, height_{height}
	, depth_ {depth}
{

}

TextureAtlas::~TextureAtlas()
{
	if(handle_) glDeleteTextures(1, &handle_);
}

unsigned int TextureAtlas::handle() const
{
	return(handle_);
}

void TextureAtlas::insertImage(ID64 handle)
{
	auto imageView = app_->getImage(handle);
	if (!imageView) return;
	auto image = imageView.get();

	if (textures_.contains(handle))
	{
		// do soemthing
		return;
	}

	PendingImage pending {imageView};

	for(auto const & quad : free_)
	{
		
	}

	pending.x;
	pending.y;

	pendingImages_.push_back(pending);

	dirty_ = true;
}

void TextureAtlas::generate()
{
	if(!dirty_) return;

	if(!handle_)
	{		
		glGenTextures(1, &handle_);
		glBindTexture(GL_TEXTURE_2D_ARRAY, handle_);
		
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 7);
		
		glTexStorage2D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width_, height_);
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, handle_);

	for(const auto & pending : pendingImages_)
	{	
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			pending.x, pending.y, pending.depth,
			pending.width, pending.height,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pending.data.data()
		);
	}

	pendingImages_.clear();
	dirty_ = false;
}

uint32_t TextureAtlas::width() const
{
	return width_;
}

uint32_t TextureAtlas::height() const
{
	return height_;
}
