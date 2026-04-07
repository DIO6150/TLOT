#include <cstring>

#include <stb/stb_image.h>

#include <resources/Texture.hpp>

#include <core/Logger.hpp>

using namespace TLOT;

Texture::Texture (unsigned char * data, size_t width, size_t height) :
	data	{data},
	width	{width},
	height	{height}
{

}

Texture::~Texture ()
{
	free (data);
}

Texture & Texture::operator= (Texture && other)
{
	if (data)
	{
		free (data);
	}

	data	= other.data;
	width	= other.width;
	height	= other.height;

	other.data = nullptr;

	return *this;
}

Texture::Texture (Texture & other):
	width {other.width},
	height {other.height}
{
	// TODO : store channels
	std::memcpy (data, other.data, sizeof (char) * width * height * 4);
}

Texture::Texture (Texture const & other):
	width {other.width},
	height {other.height}
{
	Logger::log (LogLevel::Info, "Texture copied ! Be careful !");
	// staying consistent with stbi_load
	data = (unsigned char *) malloc (width * height * 4);
	// TODO : store channels
	std::memcpy (data, other.data, sizeof (char) * width * height * 4);
}
