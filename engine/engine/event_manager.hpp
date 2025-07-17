#pragma once

#include <assert.h>
#include <stdint.h>

#include <algorithm>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <details/event.hpp>

namespace Engine {
	class EventManager {
		std::unordered_map<uint64_t, std::function<void()>>				m_registered_event;
		std::unordered_map<uint64_t, std::shared_ptr<EngineDetail::IListenerArray>> 	m_listeners;
		std::unordered_map<uint64_t, std::shared_ptr<EngineDetail::IEventQueue>> 	m_event_pool;
		std::unordered_map<uint64_t, std::shared_ptr<EngineDetail::IEventQueue>> 	m_event_pool_back;

		public:
		EventManager () {

		}

		template<typename Event>
		void RegisterEvent () {
			uint64_t uuid;
			uuid = typeid(Event).hash_code ();

			std::shared_ptr<EngineDetail::ListenerArray<Event>> 	listener;
			std::shared_ptr<EngineDetail::EventQueue<Event>> 	queue;
			std::shared_ptr<EngineDetail::EventQueue<Event>> 	queue_back;

			listener 	= std::make_shared<EngineDetail::ListenerArray<Event>> ();
			queue		= std::make_shared<EngineDetail::EventQueue<Event>> ();
			queue_back	= std::make_shared<EngineDetail::EventQueue<Event>> ();

			m_listeners		.insert ({uuid, listener});
			m_event_pool		.insert ({uuid, queue});
			m_event_pool_back	.insert ({uuid, queue_back});

			m_registered_event	.insert ({uuid, [&, uuid, listener, queue, queue_back] () -> void {
				queue->Swap (queue_back);

				while (!queue->IsEmpty ()) {
					Event event = queue->Pop ();
					listener->Execute (event);
				}
			}});
		}

		template<typename Event>
		void Subscribe (const Listener<Event> && listener, ListenPriority priority) {
			uint64_t uuid;
			uuid = typeid(Event).hash_code ();

			assert ((m_registered_event.find (uuid) != m_registered_event.end ()));

			std::static_pointer_cast<EngineDetail::ListenerArray<Event>> (m_listeners[uuid])->RegisterListener ({listener, priority});
		}

		template<typename Event>
		void PostDeferred (const Event && event) {
			uint64_t uuid;
			uuid = typeid(Event).hash_code ();

			assert ((m_registered_event.find (uuid) != m_registered_event.end ()));

			std::static_pointer_cast<EngineDetail::EventQueue<Event>> (m_event_pool_back[uuid])->Push (event);
		};

		template<typename Event>
		void PostImmediate (const Event && event) {
			uint64_t uuid;
			uuid = typeid(Event).hash_code ();

			assert ((m_registered_event.find (uuid) != m_registered_event.end ()));

			std::static_pointer_cast<EngineDetail::ListenerArray<Event>> (m_listeners[uuid])->Execute (event);
		};

		void ProcessEvents () {
			for (auto &[uuid, event_queue] : m_event_pool) {
				m_registered_event[uuid] ();
			}
		}

	};
}