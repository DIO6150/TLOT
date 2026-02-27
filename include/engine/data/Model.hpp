#pragma once

#include <vector>

#include <engine/core/ResourceManager.hpp>

#include <engine/utils/Types.hpp>

namespace Engine::Data {	
	struct Model {
		std::vector<Engine::Core::HandleID> meshes;
		
		std::string name;
	};
}