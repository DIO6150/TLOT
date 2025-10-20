#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include <initializer_list>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#define MAX_ENTITIES 99999u


namespace Engine {


	struct Entity {
		uint64_t entity_id;
		uint64_t archetype_id;
		uint64_t archetype_index;

		Entity (uint64_t entity_id) : entity_id {entity_id} {
			archetype_id = 0;
			archetype_index = 0;
		}

	};

	struct ComponentArray {
		void * 			m_components_ptr;
		uint32_t 		m_component_count;
		size_t	 		m_component_size;

		ComponentArray (size_t size) : m_component_count {0}, m_component_size {size} {
			m_components_ptr = (void *) malloc (m_component_size * MAX_ENTITIES * sizeof (char));
		}

		template<typename T>
		T & Get (size_t index) {
			assert (sizeof (T) == m_component_size);
			assert (index > 0 && index < m_component_count);
			return ((T *)m_components_ptr[index]);
		}

		template<typename T>
		size_t InsertComponent (Entity entity, const T & component) {
			assert (sizeof (T) == m_component_size);

			if (m_component_count + 1 >= m_component_size) {
				// TODO : ERROR
			}

			memcpy (m_components_ptr + m_component_size * m_component_count, &component, sizeof (component))

			++m_component_count;
		}
	};

	struct ComponentInfo {
		size_t		m_size;
		uint64_t	m_id;

		template<class T>
		ComponentInfo () : m_size {sizeof (T)}, m_id {typeid (T)} {
				
		}
	};

	class Archetype {
		private:
		std::unordered_map<uint64_t, ComponentArray> m_components;


		public:

		Archetype (std::initializer_list<ComponentInfo> infos) {
			m_components.reserve (16); // DISCLAIMER : arbitrary value
			size_t index = 0;
			for (auto & info : infos) {
				m_components.emplace (info.m_id, info.m_size);
			}
		}

		template<class T>
		void AddComponent (Entity & entity, const T && component) {
			ComponentInfo info {component};

			assert (m_component_ids.find (info.m_id) != m_component_ids.end ());


		}

		template<class ... T>
		void AddEntity (Entity & entity, std::vector<ComponentInfo> infos, const T && ... components) {
			int index = 0;
			for (const auto & component : {components ...}) {
				ComponentInfo & info = infos[index];
				ComponentArray & array = m_components.at (info.m_id);

				

				++index;
			}
		}
	};

	class ComponentManager {
		private:
		std::unordered_map<uint32_t, Archetype> m_archetypes;
		std::vector<bool> 			m_entity_ids;

		uint64_t GenerateArchetypeID (const std::vector<ComponentInfo> & infos) {
			uint64_t archetype_id;
			archetype_id = 0;

			for (const auto & info : infos) {
				archetype_id ^= info.m_id;
			}
		}

		Archetype & GetArchetype (uint64_t archetype_id) {
			auto pos = m_archetypes.find (archetype_id);
			
			assert (pos != m_archetypes.end ());
			return (pos->second);
		}

		Archetype & GenerateArchetype (const std::vector<ComponentInfo> & infos) {
			uint64_t archetype_id = GenerateArchetypeID (infos);

			return (m_archetypes.emplace (archetype_id, infos).first->second);
		}

		template<class ... T>
		std::vector<ComponentInfo> GenerateComponentInfos (T ... components) {
			std::vector<ComponentInfo> infos;

			for (const auto & component : {components...}) {
				infos.push_back (ComponentInfo<decltype (component)> {})
			}

			return (infos);
		}

		public:
		ComponentManager () {
			for (uint64_t id = 0; id < MAX_ENTITIES; ++id) {
				m_entity_ids.push_back (false);
			}
		}

		template<class ... T>
		Entity GenerateEntity (T ... components) {
			uint64_t entity_id;
			uint64_t archetype_id;
			
			// Id cannot be 0 so we start at 1
			for (uint64_t id = 1; id < MAX_ENTITIES; ++id) {
				if (m_entity_ids[id]) { entity_id = id; break; }
			}

			Entity ent {entity_id};
			std::vector<ComponentInfo> infos = GenerateComponentInfos (components ...);

			Archetype & archetype = GenerateArchetype (infos);

			archetype.AddEntity (ent, infos, components ...);

			return (ent);
		}
	};
}