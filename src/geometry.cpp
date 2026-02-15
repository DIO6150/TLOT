#include <engine/data/Geometry.hpp>
#include <engine/utils/ShortcutTypes.hpp>

using namespace Engine::Data;

Geometry::Geometry () {

}

Geometry::Geometry (Vector<Vertex> & vert, Vector<uint32_t> & ind) {
	vert.swap (vertices);
	ind .swap (indices);	
}