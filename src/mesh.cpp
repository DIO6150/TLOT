#include <engine/data/Mesh.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

using namespace Engine::Core;
using namespace Engine::Data;

Mesh::Mesh (GeometryID geometry, MaterialID material):
	geometry {geometry},
	material {material},
	position {0.0f},
	rotation {0.0f},
	scale	 {1.0f} {

}

Mesh::Mesh (GeometryID geometry, MaterialID material, glm::mat4 transform):
	geometry {geometry},
	material {material},
	position {0.0f},
	rotation {0.0f},
	scale	 {1.0f} {
		glm::vec3 a;
		glm::vec4 c;

		glm::quat rot;
		glm::decompose (transform, scale, rot, position, a, c);

		rotation = glm::degrees (glm::eulerAngles (rot));
}

