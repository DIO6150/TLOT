#pragma once

#include <string>

namespace TLOT
{
	class ShaderSource
	{
	public:
		std::string source;	

		ShaderSource (std::string source);

		ShaderSource () = default;
		ShaderSource (ShaderSource & other) = default;
		ShaderSource (ShaderSource const & other) = default;
		ShaderSource (ShaderSource && other) = default;
		ShaderSource & operator= (ShaderSource & other) = default;
		ShaderSource & operator= (ShaderSource const & other) = default;
		ShaderSource & operator= (ShaderSource && other) = default;

		~ShaderSource () = default;
	};
}
