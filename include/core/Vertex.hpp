#pragma once

#include <ostream>

namespace TLOT
{
	struct Vertex
	{
		float px, py, pz;
		float nx, ny, nz;
		float tx, ty, tz;
		float u, v;

		// TODO: add this elsewhere to keep class clean
		friend std::ostream& operator<< (std::ostream &out, const Vertex & data)
		{
			out << "([" << data.px << ", " << data.py << ", " << data.pz << "], [" <<
			data.nx << ", " << data.ny << ", " << data.nz << "], [" <<
			data.tx << ", " << data.ty << ", " << data.tz << "], [" <<
			data.px << ", " << data.py << "])";
			return out;
		}
	};
}