#pragma once

#include <engine/core/AssetManager.hpp>
#include <engine/core/ResourceManager.hpp>
#include <engine/data/InstancedData.hpp>

#include <engine/utils/Types.hpp>

namespace Engine::Core {
	class IRenderer {
	public:
		virtual void PushModel (Instance<Model> & model, const HandleID & shaderID) = 0;
		virtual void Render (Module::Camera * camera) = 0;
	};
}