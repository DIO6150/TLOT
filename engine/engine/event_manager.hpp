#pragma once

#include <assert.h>
#include <stdint.h>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>

#include <details/event.hpp>

namespace Engine {
	class EventManager {
		std::vector<uint64_t> 								m_registered_event;
		std::unordered_map<uint64_t, std::shared_ptr<EngineDetail::IListenerArray>> 	m_listeners;
		std::unordered_map<uint64_t, EngineDetail::IEventQueue> 			m_event_pool;

		public:
		EventManager () {

		}

		template<typename Event>
		void RegisterEvent () {
			uint64_t uuid;
			uuid = typeid(Event).hash_code ();

			m_registered_event.push_back (uuid);
		}

		template<typename Event>
		void Subscribe (const Listener<Event> && listener, ListenPriority priority) {
			uint64_t uuid;
			uuid = typeid(Event).hash_code ();

			if (std::find (m_registered_event.begin (), m_registered_event.end (), uuid) == m_registered_event.end ()) {
				m_listeners.insert ({uuid, std::make_shared<EngineDetail::ListenerArray<Event>> ()});
			}

			static_cast<std::shared_ptr<EngineDetail::ListenerArray<Event>>> (m_listeners[uuid])->RegisterListener ((EngineDetail::ListenerWrapper<Event>){listener, priority});
		}

		template<typename Event>
		void PostDeferred (const Event && event) {
			uint64_t uuid;
			uuid = typeid(Event).hash_code ();

			assert(std::find (m_registered_event.begin (), m_registered_event.end (), uuid) != m_registered_event.end ());

			static_cast<EngineDetail::EventQueue<Event>*> (m_event_pool[uuid])->Push (event);
		};

		template<typename Event>
		void PostImmediate (const Event && event) {
			uint64_t uuid;
			uuid = typeid(Event).hash_code ();

			assert(std::find (m_registered_event.begin (), m_registered_event.end (), uuid) != m_registered_event.end ());

			static_cast<std::shared_ptr<EngineDetail::ListenerArray<Event>>> (m_listeners[uuid])->Execute (event);
		};

	};
}