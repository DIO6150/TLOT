#pragma once

#include <assert.h>
#include <unordered_map>
#include <typeinfo>

#include <utils.hpp>

#include <details/component.hpp>

#include <vendor/glm/glm.hpp>
#include <vendor/glm/matrix.hpp>
#define  GLM_ENABLE_EXPERIMENTAL
#include <vendor/glm/gtx/transform.hpp>
#include <vendor/glad/include/glad/glad.h>

#include <memory>

namespace Engine {
	class ComponentManager {
		std::unordered_map<uint64_t, std::shared_ptr<EngineDetail::IComponentArray>> m_component_arrays;
		
		template<typename T>
		uint64_t CreateComponentArray () {
			uint64_t hash;

			hash = typeid (T).hash_code ();

			if (m_component_arrays.find (hash) == m_component_arrays.end ()) {
				m_component_arrays.insert ({hash, std::make_shared<EngineDetail::ComponentArray<T>> ()});
			}
			
			return (hash);
		}

		template<typename T>
		EngineDetail::ComponentArray<T> * GetComponentArray (uint64_t uuid) {
			assert(m_component_arrays.find (uuid) != m_component_arrays.end ());

			return (static_cast<EngineDetail::ComponentArray<T> *>(m_component_arrays[uuid]));
		}

		public:
		ComponentManager () {
			m_component_arrays.reserve (COMPONENT_ARRAYS_DEFAULT_SIZE);
		}

		template<typename T>
		void AddComponent (uint32_t entity, const T & component) {
			uint64_t uuid;
			uuid = CreateComponentArray<T> ();

			GetComponentArray<T> (uuid)->AddEntity (entity, component);
		}

		template<typename T>
		void RemoveComponent (uint32_t entity) {
			uint64_t uuid;
			uuid = typeid (T).hash_code ();

			GetComponentArray<T> (uuid)->RemoveEntity (entity);
		}

		template<typename T>
		T * GetFirst () {
			uint64_t uuid;
			uuid = typeid (T).hash_code ();

			return (GetComponentArray<T> (uuid)->GetContainer ().data ());
		}
	};
}