#include <engine/instance_data.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


Engine::InstanceData::InstanceData () :
	matrix {1.0f},
	atlasCoords {0.0, 0.0, 1.0, 1.0}
{

}

Engine::InstanceData::InstanceData (
		glm::vec3 position, 
		glm::vec3 rotation,
		glm::vec3 scale,
		glm::vec4 coords
	) : InstanceData {} {

	matrix = glm::scale (matrix, scale);
	matrix = glm::rotate (matrix, rotation.z, glm::vec3 (0.0, 0.0, 1.0));
	matrix = glm::rotate (matrix, rotation.y, glm::vec3 (0.0, 1.0, 0.0));
	matrix = glm::rotate (matrix, rotation.x, glm::vec3 (1.0, 0.0, 0.0));
	matrix = glm::translate (matrix, position);

	atlasCoords = coords;
}