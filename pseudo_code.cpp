
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <set>
#include <deque>

#include "vendor/glm/glm.hpp"
#include "vendor/glm/matrix.hpp"
#include "vendor/glm/gtx/transform.hpp"
#include "vendor/glad/include/glad/glad.h"
#include "utils.hpp"
#include "mesh.hpp"

#define COMPONENTS_DEFAULT_SIZE 16
#define COMPONENT_ARRAYS_DEFAULT_SIZE 16

// necessary to hold an array of different templated arguments
class IComponentArray {
	
};

template<typename T>
class ComponentArray : IComponentArray {
	private:
	std::vector<T> m_components;
	std::unordered_map<uint32_t, size_t> m_entity_index_map;
	std::unordered_map<size_t, uint32_t> m_index_entity_map;

	public:
	ComponentArray () {
		m_components.reserve (COMPONENTS_DEFAULT_SIZE);
	}

	void AddEntity (uint32_t entity, const T & component) {
		m_components.push_back (T);
		m_entity_index_map.insert ({entity, m_components.size ()});
		m_index_entity_map.insert ({m_components.size (), entity});
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
		std::unordered_map<uint32_t, size_t>::iterator 	pos;
		size_t 						index;

		assert((pos = m_entity_index_map.find (entity)) != m_entity_index_map.end ());

		index = pos->second;
		return (m_components[index]);
	}

	const std::vector<T> & GetContainer () {
		return (m_components);
	}
};

class ComponentManager {
	std::unordered_map<uint64_t, IComponentArray*> m_component_arrays;
	
	template<typename T>
	uint64_t CreateComponentArray () {
		uint64_t hash;

		hash = typeid (T).hash_code ();

		if (m_component_arrays.find (hash) == m_component_arrays.end ()) {
			m_component_arrays.insert ({hash, new ComponentArray<T> {}})
		}
		
		return (hash);
	}

	template<typename T>
	ComponentArray<T> * GetComponentArray (uint64_t uuid) {
		assert(m_component_arrays.find (uuid) != m_component_arrays.end ());

		return (static_cast<ComponentArray<T> *>(m_component_arrays[uuid]));
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

class EntityManager {
	std::unordered_map<uint32_t, uint32_t> m_entities;
};


enum class EventResult {
	COMPLETE,
	CONTINUE,
	CANCEL
};

enum class ListenPriority {
	HIGHEST = 0x1,
	HIGH	= 0x4,
	MEDIUM	= 0x8,
	LOW	= 0x10,
	LOWEST	= 0x20
};

class EventProperties {

};

template<typename E>
using Listener = std::function<EventResult(E &event)>;

template<typename E>
struct ListenerWrapper {
	Listener && 		listener;
	ListenPriority		priority;

	bool operator< (const ListenerWrapper<E> & other) {
		return (priority < other.priority)
	}
};

class IListenerArray {

};

template<typename E>
class ListenerArray : IListenerArray {
	std::multiset<ListenerWrapper<E>> m_listeners;

	ListenerArray () {
		
	}

	std::multiset<ListenerWrapper<E>>::iterator RegisterListener (ListenerWrapper<E> &&wrapper) {
		return (m_listeners.insert (wrapper));
	}

	void UnregisterListener (std::multiset<ListenerWrapper<E>>::iterator pos) {
		m_listeners.erase (pos);
	}

	void UnregisterAll (ListenPriority priority) {
		m_listeners.erase (m_listeners.begin (), m_listeners.lower_bound (priority));
	}
};

class IEventQueue {

};

template<typename E>
class EventQueue : IEventQueue {
	std::deque<E> m_events;

	public:
	void Push (const Event & event) {
		m_events.push_back (event);
	}

	E && Pop  () {
		E && event = m_events.back ();
		m_events.pop_back ();

		return (event);
	}
};

class EventManager {
	std::vector<uint64_t> m_registered_event;
	std::unordered_map<uint64_t, IListenerArray *> m_listeners;
	std::unordered_map<uint64_t, IEventQueue> m_event_pool;

	public:
	EventManager () {

	}

	template<typename E>
	void RegisterEvent () {
		uint64_t uuid;
		uuid = compute_hash (typeid(E).name);

		m_registered_event.push_back (uuid);
	}

	template<typename E>
	void Subscribe (const Listener<E> && listener) {
		Subscribe (listener, ListenPriority::MEDIUM)
	}

	template<typename E>
	void Subscribe (const Listener<E> && listener, ListenPriority priority) {
		uint64_t uuid;
		uuid = compute_hash (typeid(E).name);

		std::unordered_map<uint64_t, IListenerArray *>::iterator pos;

		assert((pos = m_registered_event.find (uuid)) != m_registered_event.end ());

		static_cast<ListenerArray<E>*> (pos->second)->RegisterListener ({listener, priority});
	}

	template<typename E>
	void Post (const E & event) {

	};

};

struct DummyEvent {
	int magic;
	int magic2;
};

void main () {
	ComponentManager m {};
	m.AddComponent<Engine::Vertex> (0, (Engine::Vertex) {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});

	EventManager em {};
	em.RegisterEvent<DummyEvent> ();


	em.Subscribe<DummyEvent> ([] (DummyEvent &event) -> EventResult {
		std::cout << "Hello World : " << event.magic << "\n";

		return (EventResult::COMPLETE);
	});
}