#include <mesh.hpp>

ED::Mesh::Mesh () : 
Mesh (
	nullptr, 
	nullptr,
	glm::vec3 {0.0},
	glm::vec3 {0.0},
	glm::vec3 {0.0}
) {

}

ED::Mesh::Mesh (
			Engine::Scene * parent,
			ED::Geometry  * geometry,
			glm::vec3	position,
			glm::vec3	rotation,
			glm::vec3	scale
		) : 
	visible 	{true},
	geometry 	{geometry},
	position	{position},
	rotation	{rotation},
	scale		{scale},
	parent 		{parent}
{

}