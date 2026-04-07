#pragma once

namespace TLOT
{
	struct Texture
	{
		unsigned char * data   = nullptr;
		size_t	        width  = 0;
		size_t	        height = 0;


		Texture () {}
		Texture (unsigned char * data, size_t width, size_t height);
		Texture (Texture &  other);
		Texture (Texture const & other);
		Texture & operator= (Texture && other);



		Texture (Texture && other) = delete;
		Texture (Texture const && other) = delete;

		Texture & operator= (Texture & other) = delete;
		Texture & operator= (Texture const & other) = delete;

		Texture & operator= (Texture const && other) = delete;
		~Texture ();
	};
}