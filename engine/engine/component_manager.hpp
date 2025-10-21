#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include <initializer_list>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include <utils.hpp>

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

	struct ComponentArrayInterface {
		
	};

	template<class T>
	class ComponentArray : ComponentArrayInterface {
		public:
		ComponentArray () : m_count (0) {

		}

		T & Get (size_t index) {
			assert (index > 0 && index < m_count);
			return (m_ptr[index]);
		}

		void Insert (Entity entity, const T & component) {
			if (m_component_count + 1 >= MAX_ENTITIES) {
				// TODO : ERROR
			}

			m_components_ptr[m_component_count] = component;

			++m_component_count;
		}

		private:
		T * 		m_ptr;
		size_t 		m_count;		

	};

	template<class ... C>
	class Archetype {
		public:

		Archetype (uint64_t uid) : m_uid (uid) {
			m_components.reserve (16); // DISCLAIMER : arbitrary value
		}

		template<class T>
		void AddComponent (Entity & entity, const T && component) {
			uint64_t uid = compute_hash (typeid (component).name ())

			assert (m_component.find (uid) != m_components.end ());

			m_components.insert ({uid, component});
		}

		template<class ... Args>
		bool AddEntity (Entity & entity, const Args && ... components) {
			uint64_t uid = 0;

			for (const auto & component : {components ...}) {
				uid ^= compute_hash (typeid (component).name);
			}

			if (uid != m_uid) return (false);

			for (const auto & component : {components ...}) {
				ComponentArray<decltype (component)> & array = m_components.at (compute_hash (typeid (component).name));
				array.Insert (entity, component);
			}

			return (true);
		}

		uint64_t GetUID () {
			return (m_uid);
		}

		private:
		uint64_t 						m_uid;
		std::unordered_map<uint64_t, ComponentArrayInterface>	m_components;
	};

	class ComponentManager {
		private:
		std::unordered_map<uint32_t, Archetype> m_archetypes;
		std::vector<bool> 			m_entity_ids;
		

		Archetype & GetArchetype (uint64_t archetype_id) {
			auto pos = m_archetypes.find (archetype_id);
			
			assert (pos != m_archetypes.end ());
			return (pos->second);
		}

		template<class ... T>
		Archetype CreateArchetype (uint64_t uid) {
			m_archetypes.insert ({uid, Archetype {uid}});
		}
		
		template<typename ... T>
		Archetype & GenerateArchetypeUID () {
			uint64_t uid;

			uid = compute_hash (typeid (T).name) ^ ...;
		}
		
		uint64_t GenerateEntityUID () {
			// Id cannot be 0 so we start at 1
			for (uint64_t id = 1; id < MAX_ENTITIES; ++id) {
				if (!m_entity_ids[id]) {
					m_entity_ids[id] = true;
					return (id);
				}
			}

			return (0);
		}

		public:
		ComponentManager () {
			m_entity_ids.reserve (MAX_ENTITIES);
			for (uint64_t id = 0; id < MAX_ENTITIES; ++id) {
				m_entity_ids.push_back (false);
			}
		}
		
		template<class ... T>
		Entity GenerateEntity (const T ... components) {
			Archetype * archetype;

			uint64_t entity_id;
			uint64_t archetype_id;
			
			entity_id 	= GenerateEntityUID ();
			archetype_id 	= GenerateArchetypeUID ();

			auto pos = m_archetypes.find (archetype_id);

			if (pos == m_archetypes.end ()) {
				
			}

			Entity ent {entity_id};
			return (ent);
		}
	};
}