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
		HIGHEST = 0x1 ,
		HIGH	= 0x4 ,
		MEDIUM	= 0x8 ,
		LOW	= 0x10,
		LOWEST	= 0x20
	};

	template<typename E>
	using Listener = std::function<EventResult(E &event)>;
}

namespace EngineDetail {
	template<typename Event>
	struct ListenerWrapper {
		Engine::Listener<Event> 	listener;
		Engine::ListenPriority		priority;

		bool operator< (const ListenerWrapper<Event> & other) const {
			return (priority < other.priority);
		}
	};

	class IListenerArray {

	};

	template<typename E>
	class ListenerArray : public IListenerArray {
		std::multiset<ListenerWrapper<E>> m_listeners;

		public:

		ListenerArray () {
			
		}

		typename std::multiset<ListenerWrapper<E>>::iterator RegisterListener (ListenerWrapper<E> &&wrapper) {
			return (m_listeners.insert (wrapper));
		}

		void UnregisterListener (typename std::multiset<ListenerWrapper<E>>::iterator pos) {
			m_listeners.erase (pos);
		}

		void UnregisterAll (Engine::ListenPriority priority) {
			m_listeners.erase (m_listeners.begin (), m_listeners.lower_bound (priority));
		}

		void Execute (const E & event) {
			for (auto & l : m_listeners) {
				Engine::EventResult status = l.listener (event);

				if (status == Engine::EventResult::CANCEL) return;
			}
		}
	};

	class IEventQueue {

	};

	template<typename Event>
	class EventQueue : IEventQueue {
		std::deque<Event> m_events;

		public:
		void 	Push (const Event && event) {
			m_events.push_back (event);
		}

		Event && Pop  () {
			Event && event = m_events.back ();
			m_events.pop_back ();

			return (event);
		}
	};
}
