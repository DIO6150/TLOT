#pragma once

#include <set>
#include <functional>

namespace Engine {
	enum class EventResult {
		COMPLETE,
		CONTINUE,
		CANCEL
	};
	
	enum class ListenPriority {
		UNCANCELABLE = 	0x0,
		HIGHEST = 	0x1 ,
		HIGH	= 	0x4 ,
		MEDIUM	= 	0x8 ,
		LOW	= 	0x10,
		LOWEST	= 	0x20
	};

	template<typename Event>
	using Listener = std::function<EventResult(const Event &event)>;
}

namespace EngineDetail {
	template<typename Event>
	struct ListenerWrapper {
		Engine::Listener<Event>	listener;
		Engine::ListenPriority	priority;

		bool operator< (const ListenerWrapper<Event> & other) const {
			return (priority < other.priority);
		}
	};

	class IListenerArray {

	};

	template<typename Event>
	class ListenerArray : public IListenerArray {
		std::multiset<ListenerWrapper<Event>> m_listeners;

		public:

		ListenerArray () {
			
		}

		void RegisterListener (ListenerWrapper<Event> &&wrapper) {
			m_listeners.insert (wrapper);
		}

		void UnregisterAll (Engine::ListenPriority priority) {
			m_listeners.erase (m_listeners.lower_bound (priority), m_listeners.end ());
		}

		void Execute (const Event & event) {
			using ListenerIt = typename std::multiset<ListenerWrapper<Event>>::iterator;
			std::vector<ListenerIt> to_erase;
			bool 			should_cancel;

			to_erase.reserve (m_listeners.size ());

			for (ListenerIt it = m_listeners.begin (); it != m_listeners.end (); ++it) {
				if (should_cancel && it->priority != Engine::ListenPriority::UNCANCELABLE) return;

				Engine::Listener<Event> listener = it->listener;
				Engine::EventResult status = listener (event);

				if (status == Engine::EventResult::COMPLETE) to_erase.push_back (it);
				if (status == Engine::EventResult::CANCEL) {
					if (it->priority == Engine::ListenPriority::UNCANCELABLE) should_cancel = true;
					else return;
				}
			}

			for (auto & it : to_erase) {
				m_listeners.erase (it);
			}
		}
	};

	class IEventQueue {
		virtual bool IsEmpty () const = 0;
		virtual void Swap (std::shared_ptr<IEventQueue> other) = 0;
	};

	template<typename Event>
	class EventQueue : public IEventQueue {
		std::deque<Event> m_events;

		public:
		void Push (const Event & event) {
			m_events.push_back (event);
		}

		Event Pop () {
			Event event = m_events.back ();
			m_events.pop_back ();

			return (event);
		}

		bool IsEmpty () const {
			return (m_events.size () == 0);
		}

		void Swap (std::shared_ptr<IEventQueue> other) {
			EventQueue<Event> & other_queue = *std::static_pointer_cast<EventQueue<Event>> (other);

			m_events.swap (other_queue.m_events);
		}
	};
}
