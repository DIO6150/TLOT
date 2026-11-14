
#include <glad/glad.h>

#include <TetoEngine/Private/Renderer/TextureAtlas.hpp>
#include <TetoEngine/Application/Application.hpp>

#include <algorithm>

#include <TetoEngine/Utils/Logger.hpp>

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
	return handle_;
}

void TextureAtlas::insertImage(ID64 handle)
{
	auto imageView = app_->getImage(handle);
	if (!imageView)
	{
		return;
	}

	PendingImage pending {imageView};
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
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 7);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width_, height_, depth_);
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, handle_);

	std::vector<std::pair<PendingImage, Quad>> quads = generateQuads();
	for(const auto & [pendingImage, quad] : quads)
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			quad.x, quad.y, quad.d,
			quad.w, quad.h, 1,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pendingImage.data.data()
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

inline bool canFit(PendingImage image, Quad quad)
{
	return quad.w >= static_cast<float>(image.width) && quad.h >= static_cast<float>(image.height);
}

inline Quad fit(PendingImage image, Quad quad)
{
	quad.w = image.width;
	quad.h = image.height;

	return quad;
}

std::vector<Quad> cutTile(Quad base, Quad cut)
{
	std::vector<Quad> result;

	float w1, h1, w2, h2, x1, y1, x2, y2;

	w1 = base.w - cut.w;
	w2 = base.w;

	h1 = base.h - cut.h;
	h2 = cut.h;

	x1 = base.x + cut.w;
	y1 = base.y;

	x2 = base.x;
	y2 = base.y + cut.h;

	if (w1 != 0.0f)
	{
		result.emplace_back(x1, y1, w1, h2, 1);
	}

	if (h1 != 0.0f)
	{
		result.emplace_back(x2, y2, w2, h1, 1);
	}

	return result;
}

std::optional<Quad> TextureAtlas::fitCut(PendingImage image)
{
	bool found = false;
	for (auto const & candidate : free_)
	{
		found = canFit(image, candidate);

		if (found)
		{
			Quad fitted = fit(image, candidate);

			auto tiles = cutTile(candidate, fitted);
			free_.insert(free_.end(), tiles.begin(), tiles.end());

			return fitted;
		}
	}

	return std::nullopt;
}

std::vector<std::pair<PendingImage, Quad>> TextureAtlas::generateQuads()
{
	std::vector<std::pair<PendingImage, Quad>> result;

	for (auto const & pending : pendingImages_)
	{
		auto opt = fitCut(pending);
		if (!opt)
		{
			Logger::log(LogLevel::Warning, "Couldn't fit image in atlas: Skipping...");
			continue;
		}

		result.emplace_back(pending, opt.value());
	}

	return result;
}