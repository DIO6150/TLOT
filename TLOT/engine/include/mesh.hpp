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
		Mesh (
			Engine::Scene * parent,
			ED::Geometry  * geometry,
			glm::vec3	position,
			glm::vec3	rotation,
			glm::vec3	scale
		);

	protected:
		bool		visible;

		Geometry *	geometry;
		Material *	material;

		glm::vec3	position;
		glm::vec3	rotation;
		glm::vec3	scale;

		Engine::Scene *	parent;

	private:
		friend class Batch;
		friend class Engine::Scene;
		friend class Engine::Engine;
	};
}