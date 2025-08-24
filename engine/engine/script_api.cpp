#include <script_api.hpp>
#include <event_manager.hpp>

#include <stdio.h>

Engine::ScriptAPIProxy::ScriptAPIProxy (EventManager * event_manager) {
	setup_func = [event_manager] (sol::state & state) {		
		state.set_function ("RegisterEvent", [event_manager](uint32_t id)				{ event_manager->RegisterEvent (id); });
		state.set_function ("PostDeferred",  [event_manager](uint32_t id, const sol::table data)	{ event_manager->PostDeferred  (Event {id, data}); });
		state.set_function ("PostImmediate", [event_manager](uint32_t id, const sol::table data)	{ event_manager->PostImmediate (Event {id, data}); });
		state.set_function ("CancelEvent",   [event_manager]()						{ event_manager->CancelEvent (); });

		state.new_enum<Engine::EventResult> (
			"EventResult", {
				{"COMPLETE", 	EventResult::COMPLETE},
				{"CONTINUE", 	EventResult::CONTINUE}
			}
		);
	};
}

void Engine::ScriptAPIProxy::Bind (sol::state & state) {
	setup_func (state);
}