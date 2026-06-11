#include <cstring>

#include <stb/stb_image.h>

#include <resources/Texture.hpp>

#include <core/Logger.hpp>

#include <stdio.h>

using namespace TLOT;

Texture::Texture(unsigned char * data, size_t width, size_t height) :
	data	{data},
	width	{width},
	height	{height}
{

}

Texture::~Texture()
{
	if (data)
	{
		Logger::log(LogLevel::Info, "destructor called !");
		free(data);
	}
}

Texture & Texture::operator=(Texture && other)
{
	if (data)
	{
		free(data);
	}

	data	= other.data;
	width	= other.width;
	height	= other.height;

	other.data = nullptr;

	return *this;
}

Texture::Texture(Texture && other)
{
	data	= other.data;
	width	= other.width;
	height	= other.height;

	other.data = nullptr;
}

Texture::Texture(Texture & other)
{
	data =(unsigned char*) malloc(4 * other.width * other.height);
	width = other.width;
	height = other.height;

	memcpy(data, other.data, width * height * 4);

	Logger::log(LogLevel::Warning, "Copying texture, this is NOT cheap(well, maybe it is)");
}

Texture::Texture(Texture const & other)
{
	data =(unsigned char*) malloc(4 * other.width * other.height);
	width = other.width;
	height = other.height;

	memcpy(data, other.data, width * height * 4);

	Logger::log(LogLevel::Warning, "Copying texture, this is NOT cheap(well, maybe it is)");
}
