#pragma once

#include <string>
#include <vector>

#include <stdint.h>

#include <glm/glm.hpp>

#include <engine/core/ResourceManager.hpp>

#include <engine/data/Instance.hpp>
#include <engine/data/Geometry.hpp>
#include <engine/data/Texture.hpp>
#include <engine/data/Vertex.hpp>

#include <engine/utils/Types.hpp>

namespace Engine::Data {
	struct Mesh {
		Mesh (Core::GeometryID geometry, Core::MaterialID material);

		Core::GeometryID geometry;
		Core::MaterialID material;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};
}