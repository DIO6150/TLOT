#pragma once

#include <stdint.h>

namespace Engine {
	struct Material {
		Shader * shader;
		TextureQuad * diffuse;
	};
}