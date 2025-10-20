#pragma once

#include <assert.h>
#include <stdint.h>

#include <algorithm>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <deque>
#include <set>
#include <functional>

#include <sol/sol.hpp>
#include <script_api.hpp>

namespace Engine {
	enum class EventResult {
		COMPLETE,
		CONTINUE
	};

	class Event {
		private:

		public:
		uint32_t	event_id;
		sol::table	event_data;

		Event (uint32_t event_id, sol::table event_data) : event_id {event_id}, event_data {event_data} {

		}
	};

	class Listener {
		public:
		uint32_t	event_id;
		std::string	function_name;
		uint32_t	priority;

		Listener (
			uint32_t 	event_id,
			std::string 	function_name,
			uint32_t 	priority) : 
		event_id 	(event_id),
		function_name 	(function_name),
		priority 	(priority) {
			
		}
		
		bool operator< (const Listener & other) const {
			return (priority < other.priority);
		}
	};

	class EventManager {
		std::unordered_map<uint32_t, std::set<Listener>>	m_listeners;
		std::deque<Event>			 		m_event_queue;
		std::deque<Event>			 		m_event_queue_back;

		std::set<std::string>					m_loaded_scripts;
		ScriptAPIProxy *					m_script_api;
		sol::state						m_state;
		bool							m_should_cancel;


		// Execute(...) should not be called recursively unless the nested calls are set to cancelable=false.
		// Otherwise it will break top-level Execute(...) (like weird shit will happen and you DONT want that)
		// The only case where Execute(...) is called recursivly is if a script calls PostImmediate, but cancelable is already set to false.
		// So be aware when extending this functionnality.
		// TODO : The way to fix this is to find a way to not break Execute when canceling an immediate event, maybe a FIFO where we push a cancel state and pop it back after script execution
		void Execute (const Event & event, bool cancelable = true) {
			bool should_continue;
			
			should_continue = false;

			for (auto & listener : m_listeners.at (event.event_id)) {
				const sol::protected_function & listener_function = m_state.get<sol::protected_function> (listener.function_name);
				if (!listener_function.valid ()) continue;

				const sol::protected_function_result & listener_result = listener_function (event.event_data);
				if (!listener_result.valid ()) continue;

				EventResult result = listener_result.get<EventResult> ();

				if (m_should_cancel && cancelable) {
					m_should_cancel = false;					
					return;
				}

				if (result == EventResult::CONTINUE) {
					should_continue = true;
				}
			}

			if (should_continue) {
				PostDeferred (event);
			}
		}

		public:
		EventManager () : m_script_api (nullptr), m_should_cancel (false) {
			m_state.open_libraries (sol::lib::base);
		}

		void RegisterScriptAPI (ScriptAPIProxy * proxy) {
			assert (proxy != nullptr);

			m_script_api = proxy;

			m_script_api->Bind (m_state);
		}

		void RegisterEvent (uint32_t event_id) {
			m_listeners.try_emplace (event_id);
		}

		void LoadListener (
			uint32_t event_id,
			const std::string & function_name,
			uint32_t priority,
			const std::string & script) {
			assert ((m_listeners.find (event_id) != m_listeners.end ()));
			assert (m_script_api != nullptr);

			m_listeners.at (event_id).emplace (event_id, function_name, priority);

			if (m_loaded_scripts.find (script) != m_loaded_scripts.end ()) return;

			m_loaded_scripts.insert (script);
			m_state.script_file (script);
		}

		void PostDeferred (const Event && event) {
			assert ((m_listeners.find (event.event_id) != m_listeners.end ()));

			m_event_queue_back.push_back (event);
		};

		void PostDeferred (const Event & event) {
			assert ((m_listeners.find (event.event_id) != m_listeners.end ()));

			m_event_queue_back.push_back (event);
		};

		void PostImmediate (const Event && event) {
			assert ((m_listeners.find (event.event_id) != m_listeners.end ()));

			Execute (event, false);
		};

		void ProcessEvents () {
			m_event_queue.swap (m_event_queue_back);

			while (!m_event_queue.empty ()) {
				const Event event = m_event_queue.back ();
				m_event_queue.pop_back ();
				Execute (event);
			}
		}

		void CancelEvent () {
			m_should_cancel = true;
		}

	};
}