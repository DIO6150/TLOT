#pragma once

#include <string>
#include <vector>

#include <stdint.h>

#include <glm/glm.hpp>

#include <geometry.hpp>
#include <vertex.hpp>
#include <instance_data.hpp>

namespace ED {
	class Mesh {
	public:
		Mesh (Geometry & geometry);

		bool		visible;
		InstanceData	data;
		Geometry *	geometry;
	};
}