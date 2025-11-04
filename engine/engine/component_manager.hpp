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
#include <details/uid_generator.hpp>

#define MAX_ENTITIES 99999u


namespace Engine {
	struct Entity {
		uint64_t entity_id;
		uint64_t archetype_id;
		uint64_t row_id;

		Entity (uint64_t entity_id, uint64_t archetype_id, uint64_t row_id) :
			entity_id {entity_id},
			archetype_id {archetype_id},
			row_id {row_id} {

		}

	};


	struct ComponentArray {
		ComponentArray (size_t component_size) : m_count (0) {
			m_ptr = malloc (component_size * MAX_ENTITIES);
		}

		~ComponentArray () {
			if (m_ptr != nullptr) free (m_ptr);
		}

		ComponentArray (const ComponentArray & other) :
			m_count {other.m_count} {
			if (this != &other || m_ptr != nullptr) {
				free (m_ptr);
				memcpy (m_ptr, other.m_ptr, m_count);
			}
		}

		ComponentArray (ComponentArray && other) :
			m_ptr {other.m_ptr},
			m_count {other.m_count} {

			other.m_ptr = malloc (m_size * MAX_ENTITIES);
			other.m_count = 0;
		}

		ComponentArray & operator= (const ComponentArray & other) {
			m_count = other.m_count;

			if (this != &other || m_ptr != nullptr) {
				free (m_ptr);
				memcpy (m_ptr, other.m_ptr, m_count);
			}

			return (*this);
		}

		ComponentArray & operator= (ComponentArray && other) {
			m_count = other.m_count;

			if (this != &other || m_ptr != nullptr) {
				free (m_ptr);
				memcpy (m_ptr, other.m_ptr, m_count);

				other.m_ptr = malloc (m_size * MAX_ENTITIES);
				other.m_count = 0;
			}

			return (*this);
		}

		template<class T>
		void Insert (const T & component) {
			assert (m_count < MAX_ENTITIES);

			memcpy (m_ptr + m_count * m_size, & component, sizeof (T));
			++m_count;
		}

		void * 		m_ptr;
		size_t 		m_count; // how many currently in the array, sync with other arrays from the same archetype
		size_t		m_size; // size of an element of the array

	};

	class Archetype {
	public:

		Archetype (uint64_t uid) : m_uid (uid) {			
			m_components.reserve (16);
		}

		template<class ... T>
		Entity GenerateEntity (T ... components) {
			if (m_components.empty ()) {
				m_components.insert ({GenerateUID<T> ()..., ComponentArray (sizeof (T)...)});
			}

			for (const auto & [uid, array] : m_components) {
				array.Insert ();
			}

			ComponentArray * array = m_components.

			([&] {

			} (), ...); // very cursed loop
		}

	private:
		uint64_t m_uid;

		std::unordered_map<uint64_t, ComponentArray> m_components;
	};

	class ComponentManager {
	public:

		ComponentManager () {

		}

		template<class T>
		void RegisterComponent () {
			GenerateUID<T> ();
		}
		
		template<class ... T>
		Entity GenerateEntity (const T ... components) {
			uint64_t uid = GenerateUID (T) ^ ...;

			Archetype * arch;

			auto pos = m_archetypes.find (uid);

			if (pos == m_archetypes.end ()) {
				arch = & m_archetypes.insert ({uid, uid}).first->second;
				arch->Generate<T...> ();
			}

			arch.
			

			return (Entity {0, 0, 0});
		}

	private:
		std::unordered_map<uint32_t, Archetype> m_archetypes;
	};
}