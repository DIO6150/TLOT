#pragma once

#include <string>
#include <vector>

#include <stdint.h>

#include <glm/glm.hpp>

#include <engine_types.hpp>
#include <geometry.hpp>
#include <vertex.hpp>
#include <instance_data.hpp>

namespace ED {
	class Mesh {
	public:
		Mesh ();

	protected:
		bool		visible;

		Geometry *	geometry;

	private:
		friend class Batch;
		friend class Engine::Engine;
	};
}