#pragma once

#include <string>
#include <utils.hpp>
#include <json/json.hpp>

#include <globals.hpp>
#include <event_manager.hpp>
#include <script_api.hpp>

namespace Engine {

	void Initialize () {
		Globals::g_event_manager = new EventManager {};
		Globals::g_script_api_proxy = new ScriptAPIProxy {Globals::g_event_manager};

		Globals::g_event_manager->RegisterScriptAPI (Globals::g_script_api_proxy);
	}

	void Cleanup () {
		delete Globals::g_event_manager;
		delete Globals::g_script_api_proxy;
	}

	Engine::EventManager & GetEventManagerInstance () {
		return (*Globals::g_event_manager);
	}

	void LoadConfig (const std::string & config) {
		std::string config_raw = ReadFile (config);
		nlohmann::json jconfig = nlohmann::json::parse (config_raw);

		// Events bit
		std::string event_config_file 	= jconfig["event_config"];
		std::string event_listener_path = jconfig["event_listener_path"];

		std::string event_config_raw = ReadFile (event_config_file);
		nlohmann::json jevent_config = nlohmann::json::parse (event_config_raw);

		for (auto & events : jevent_config) {
			uint32_t event_id = events["id"];
			nlohmann::json listeners = events["listeners"];
			Globals::g_event_manager->RegisterEvent (event_id);

			int priority = 0;
			for (auto & listener : listeners) {
				std::string name = listener["name"];
				std::string script = listener["script"];
				Globals::g_event_manager->LoadListener (event_id, name, priority, event_listener_path + script);
				++priority;
			}
		}
	}
}