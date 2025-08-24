#pragma once

#include <functional>

#include <sol/sol.hpp>


namespace Engine {
	class EventManager;
	
	class ScriptAPIProxy {
		private:
		std::function<void (sol::state & state)> setup_func;

		public:
		ScriptAPIProxy (EventManager * event_manager);
		void Bind (sol::state & state);
	};
}
