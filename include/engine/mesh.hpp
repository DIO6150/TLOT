#pragma once

#include <string>
#include <vector>

#include <stdint.h>

#include <glm/glm.hpp>

#include <engine/instance_data.hpp>
#include <engine/geometry.hpp>
#include <engine/texture.hpp>
#include <engine/vertex.hpp>

#include <engine/engine_types.hpp>


namespace Engine {
	class Mesh {
	public:
		Mesh ();

		void translate	(glm::vec3 offset);
		void rotate	(glm::vec3 offset);
		void scale	(glm::vec3 offset);

		Mesh (
			Scene *		scene,
			Geometry *	geometry,
			glm::vec3	position,
			glm::vec3	rotation,
			glm::vec3	scale,
			Material *   	material
		);
	private:

		bool		visible;

		Geometry *	geometry;

		glm::vec3	m_position;
		glm::vec3	m_rotation;
		glm::vec3	m_scale;

		Scene *		p_scene;

		Material *	material;

		friend class Batch;
		friend class Scene;
		friend class Engine;
	};
}