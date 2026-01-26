#include <engine/geometry.hpp>

Engine::Geometry::Geometry () {

}

Engine::Geometry::Geometry (std::vector<Vertex> vert, std::vector<uint32_t> ind) {
	vert.swap (vertices);
	ind.swap (indices);

	//printf ("(Geometry) vert size = %lld\n", vertices.size ());
	
}