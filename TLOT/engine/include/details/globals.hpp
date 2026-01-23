#pragma once

namespace Engine {
	class EventManager;
	class ScriptAPIProxy;
}

namespace Globals {
	extern Engine::EventManager 	* g_event_manager;
	extern Engine::ScriptAPIProxy 	* g_script_api_proxy;
};