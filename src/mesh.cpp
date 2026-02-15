#include <engine/data/Mesh.hpp>

using namespace Engine::Core;
using namespace Engine::Data;

Mesh::Mesh (GeometryID geometry, MaterialID material):
	geometry {geometry},
	material {material},
	position {0.0f},
	rotation {0.0f},
	scale	 {1.0f} {

}