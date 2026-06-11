#pragma once

namespace TLOT
{
	class Texture
	{
	public:
		
		unsigned char * data   = nullptr;
		size_t	        width  = 0;
		size_t	        height = 0;
	
		Texture(unsigned char * data, size_t width, size_t height);
		~Texture();
		
		Texture(Texture && other);
		Texture & operator=(Texture && other);
		Texture(Texture & other);
		Texture(Texture const & other);
		
		Texture() = default;
		Texture & operator=(Texture const & other) = default;
		Texture & operator=(Texture & other) = default;

	};
}
