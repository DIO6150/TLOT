#pragma once
#include <ostream>

namespace Engine::Data {
	struct Vertex {
		float px, py, pz;
		float nx, ny, nz;
		float tx, ty, tz;
		float u, v;

		friend std::ostream& operator<< (std::ostream &out, const Vertex & data) {
			out << "([" << data.px << ", " << data.py << ", " << data.pz << "], [" <<
			data.nx << ", " << data.ny << ", " << data.nz << "], [" <<
			data.tx << ", " << data.ty << ", " << data.tz << "], [" <<
			data.px << ", " << data.py << "])";
			return out;
		}
	};
}