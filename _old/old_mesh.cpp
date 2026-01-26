#include "mesh.hpp"

Engine::Vertex::Vertex (
	float x,
	float y,
	float z,
	float u,
	float v,
	float nx,
	float ny,
	float nz) : 
	x (x), y (y), z (z),
	u (u), v (v),
	nx (nx), ny (ny), nz (nz) {

}