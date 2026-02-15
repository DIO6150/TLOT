#pragma once

#include <stdint.h>

#include <engine/core/ResourceManager.hpp>

#include <engine/utils/Types.hpp>

#include <glm/glm.hpp>

namespace Engine::Data {
	struct Material {
		Core::ShaderID shader;
		glm::vec3 color;
		std::vector<Core::TextureID> diffuse_textures; // will only use the first one or the next if resource become invalid I guess

		Material (Core::ShaderID shader, std::vector<Core::TextureID> diffuses):
			shader {shader},
			color  {1.0}
			{
			diffuse_textures.swap (diffuses);
		}

		Material (Core::ShaderID shader, std::vector<Core::TextureID> diffuses, glm::vec3 color):
			shader {shader},
			color  {color}
			{
			diffuse_textures.swap (diffuses);
		}
	};
}