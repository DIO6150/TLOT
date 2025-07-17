#pragma once

#define COMPONENTS_DEFAULT_SIZE 16
#define COMPONENT_ARRAYS_DEFAULT_SIZE 16

#include <vector>
#include <unordered_map>
#include <stdint.h>

#include <assert.h>

namespace EngineDetail {
	// necessary to hold an array of different templated arguments
	class IComponentArray {
		
	};

	template<typename T>
	class ComponentArray : public IComponentArray {
		private:
		std::vector<T> 				m_components;
		std::unordered_map<uint32_t, size_t>	m_entity_index_map;
		std::unordered_map<size_t, uint32_t>	m_index_entity_map;

		public:
		ComponentArray () {
			m_components.reserve (COMPONENTS_DEFAULT_SIZE);
		}

		void AddEntity (uint32_t entity, const T & component) {
			m_components		.push_back (component);
			m_entity_index_map	.insert    ({entity, m_components.size ()});
			m_index_entity_map	.insert    ({m_components.size (), entity});
		}

		void RemoveEntity (uint32_t entity) {
			size_t 						index;
			size_t 						index_last;
			uint32_t					entity_last;
			std::unordered_map<uint32_t, size_t>::iterator 	pos;

			assert((pos = m_entity_index_map.find (entity)) == m_entity_index_map.end ());

			index 		= pos->second;
			index_last 	= m_components.size () - 1;
			entity_last 	= m_index_entity_map[index_last];

			m_components[index]		= m_components.back ();
			m_index_entity_map[index]	= entity_last;
			m_entity_index_map[entity_last]	= index;
			
			m_components		.pop_back ();
			m_entity_index_map	.erase (entity);
			m_index_entity_map	.erase (index_last);
		}

		T & GetComponent (uint32_t entity) const {
			std::unordered_map<uint32_t, size_t>::const_iterator 	pos;
			size_t 							index;

			assert((pos = m_entity_index_map.find (entity)) != m_entity_index_map.end ());

			index = pos->second;
			return (m_components[index]);
		}

		const std::vector<T> & GetContainer () {
			return (m_components);
		}
	};
}