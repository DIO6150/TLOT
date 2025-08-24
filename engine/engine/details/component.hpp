#pragma once

#include <set>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>

#include <assert.h>
#include <stdint.h>

namespace Engine {
	struct Entity {
		uint64_t 		id;
		uint64_t 		archetype;
		std::set<uint64_t> 	components;

		Entity () {

		}

		Entity (uint64_t id) : id (id) {

		}

		bool operator== (const Entity & other) const {
			return (id == other.id);
		}

		bool operator< (const Entity & other) const {
			return (id < other.id);
		}
	};
}

template<>
struct std::hash<Engine::Entity> {
	size_t operator() (const Engine::Entity & ent) const {
		return (static_cast<size_t> (ent.id));
	}
};

namespace EngineDetail {
	// necessary to hold an array of different templated arguments
	class IComponentArray {
		public:
		virtual void RemoveComponent (size_t index) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray {
		public:

		ComponentArray () {
			m_components.reserve (MAX_ENTITIES);
		}

		void AddComponent (size_t index, const T & component) {
			assert (index < MAX_ENTITIES);

			m_components.insert (m_components.begin () + index, component);
		}

		void RemoveComponent (size_t index) {
			assert (index < m_components.size ());

			m_components[index] = m_components.back ();
			
			m_components.pop_back ();
		}

		T & GetComponent (size_t index) {
			assert (index < m_components.size ());
			return (m_components[index]);
		}

		const std::vector<T> & GetContainer () {
			return (m_components);
		}

		private:

		std::vector<T> 	m_components;

	};

	class Archetype {
		public:

		template<typename ... T>
		struct exclude_components_t {};

		template<typename ... T>
		struct include_components_t {};

		Archetype () : m_signature (0), m_is_signature_valid (false), m_entities_count (0) {
			
		}

		const uint64_t & GetSignature () const {
			return (m_signature);
		}

		template<typename ... ComponentsT>
		void RegisterComponents () {
			(RegisterComponentImpl<ComponentsT> (), ...);
			GenerateSignature ();
		}

		template<typename ... ComponentsT>
		void UnregisterComponents () {
			(UnregisterComponentImpl<ComponentsT> (), ...);
			GenerateSignature ();
		}

		template<typename ... ComponentsT>
		void InsertComponents(Engine::Entity & entity, const ComponentsT &... components) {
			size_t	index;

			index = GenerateIndex (entity);
			(InsertComponentImpl<ComponentsT> (entity, index, components), ...);
		}

		void RemoveComponentsOf (Engine::Entity & entity) {
			assert (entity.archetype == m_signature);

			size_t		index;
			size_t		index_last;
			Engine::Entity	entity_last;

			index 		= m_entity_index_map.at (entity);
			index_last 	= m_entities_count - 1;
			entity_last 	= m_index_entity_map.at (index_last);

			for (auto & [uuid, component_array] : m_component_arrays) {
				component_array->RemoveComponent (index);
				entity.components.erase (uuid);
			}

			m_index_entity_map[index] 	= entity_last;
			m_entity_index_map[entity_last] = index;

			m_index_entity_map.erase (index_last);
			m_entity_index_map.erase (entity);

			-- m_entities_count;
		}

		template<typename ComponentT>
		ComponentT & GetComponent (Engine::Entity entity) const {
			assert (entity.archetype == m_signature);
			
			uint64_t	uuid;
			size_t		index;
			
			uuid 	= static_cast<uint64_t> (typeid (ComponentT).hash_code ());
			index 	= m_entity_index_map.at (entity);

			auto pos = m_component_arrays	 .find (uuid);
			assert (pos == m_component_arrays.end ());

			return (std::static_pointer_cast<ComponentArray<ComponentT>> (pos->second)->GetComponent (index));
		}

		template<typename T1, typename T2>
		Archetype ShallowDuplicate ();

		template<typename ... ExcludesT, typename ... IncludesT>
		Archetype ShallowDuplicate (exclude_components_t<ExcludesT ...>, include_components_t<IncludesT ...>) {
			Archetype res {};
			std::vector<uint64_t> excludes_uuid = { static_cast<uint64_t>(typeid (ExcludesT).hash_code ())... };

			for (auto & [key, value] : m_component_arrays) {
				if (std::find (excludes_uuid.begin (), excludes_uuid.end (), key) == excludes_uuid.end ()) {
					m_recreate_component_callback[key] (res);
				}
			}
			
			res.RegisterComponents<IncludesT...> ();
			res.GenerateSignature ();

			return (res);
		}

		// TODO:
		// Plutot que de chercher si il a les types, lui demander de chercher le type et s'il l'a pas, on skip, ça devrait être plus rapide (aussi on itère l'autre archétype)
		void CopyEntityFrom (const Archetype & other, Engine::Entity entity) {
			assert (entity.archetype == other.m_signature);

			for (auto &[id, component_array] : m_component_arrays) {
				auto other_array_iter = other.m_component_arrays.find (id);
				if (other_array_iter == other.m_component_arrays.end ()) continue;

				m_copy_component_callback[id] (component_array, other_array_iter->second, GenerateIndex (entity), other.m_entity_index_map.at (entity));
			}
		}

		template<typename ... C>
		void CopyEntityFromBut (const Archetype & other, Engine::Entity entity) {
			assert (entity.archetype == other.m_signature);

			std::vector<uint64_t> excludes_uuid = { static_cast<uint64_t>(typeid (C).hash_code ())... };

			for (auto &[id, component_array] : m_component_arrays) {
				auto other_array_iter = other.m_component_arrays.find (id);
				if (other_array_iter == other.m_component_arrays.end ()) continue;
				if (std::find (excludes_uuid.begin (), excludes_uuid.end (), id) != excludes_uuid.end ()) continue;

				m_copy_component_callback[id] (component_array, other_array_iter->second, GenerateIndex (entity), other.m_entity_index_map.at (entity));
			}
		}

		void PrintSpec () {
			std::printf ("        Archetype with signature : %llu\n", m_signature);
			std::printf ("        Number of Components : %llu\n", m_component_arrays.size ());
			std::printf ("        Number of entities : %llu\n", m_entities_count);
		}

		private:
		
		void GenerateSignature () {
			m_signature = 0;
			for (auto & [key, value] : m_component_arrays) {
				m_signature ^= key;
			}
		}

		size_t GenerateIndex (Engine::Entity & entity) {
			if (entity.archetype == m_signature) return (m_entity_index_map.at (entity));

			size_t	index;

			index = m_entities_count;
			
			m_entity_index_map.insert ({entity, index});
			m_index_entity_map.insert ({index, entity});

			entity.archetype = m_signature;

			++ m_entities_count;

			return (index);
		}

		template<typename T>
		void RegisterComponentImpl () {
			uint64_t uuid;
			uuid = static_cast<uint64_t> (typeid (T).hash_code ());

			assert (m_component_arrays.find (uuid) == m_component_arrays.end ());

			m_component_arrays.insert ({uuid, std::make_shared<ComponentArray<T>> ()});
			m_recreate_component_callback.insert ({uuid, [] (Archetype & other) -> void {
				other.RegisterComponentImpl<T> ();
			}});

			m_copy_component_callback.insert ({uuid, [&] (std::shared_ptr<IComponentArray> array, std::shared_ptr<IComponentArray> other_array, size_t index, size_t other_index) -> void {
				T & comp = std::static_pointer_cast<ComponentArray<T>> (other_array)->GetComponent (other_index);
				std::static_pointer_cast<ComponentArray<T>> (array)->AddComponent (index, comp);
			}});
		}

		template<typename T>
		void UnregisterComponentImpl () {
			uint64_t uuid;
			uuid = static_cast<uint64_t> (typeid (T).hash_code ());

			assert (m_component_arrays.find (uuid) != m_component_arrays.end ());

			m_component_arrays.erase (uuid);
			m_recreate_component_callback.erase (uuid);
		}

		template<typename T>
		void InsertComponentImpl (Engine::Entity & entity, size_t index, const T & component) {
			uint64_t uuid;
			uuid = static_cast<uint64_t> (typeid (T).hash_code ());

			auto pos = m_component_arrays	 .find (uuid);
			assert (pos != m_component_arrays.end ());

			entity.components.insert (uuid);
			std::static_pointer_cast<ComponentArray<T>>(pos->second)->AddComponent (index, component);
		}



		uint64_t 								m_signature;
		bool									m_is_signature_valid;

		std::unordered_map<uint64_t, std::shared_ptr<IComponentArray>>		m_component_arrays;
		std::unordered_map<Engine::Entity, size_t>				m_entity_index_map;
		std::unordered_map<size_t, Engine::Entity>				m_index_entity_map;

		size_t									m_entities_count;
		
		// je voulais écrire "decltype ([](Archetype & other) -> void {}" à la place de "std::function" mais mon avocat m'a menacé avec un épée sous la gorge pour pas que je le fasse
		std::unordered_map<uint64_t, std::function<void(Archetype & other)>>	m_recreate_component_callback; 
		std::unordered_map<uint64_t, std::function<void(
			std::shared_ptr<IComponentArray> array,
			std::shared_ptr<IComponentArray> other_array,
			size_t index,
			size_t other_index)>>						m_copy_component_callback; 
	};
}