#pragma once

#include <engine/core/AssetManager.hpp>
#include <engine/core/ResourceManager.hpp>
#include <engine/core/SceneGraph.hpp>

#include <engine/utils/Types.hpp>

namespace Engine::Core {
	class IRenderer {
	public:
		IRenderer (Scene * scene);

		virtual ~IRenderer () = 0;

		virtual void Init (int width, int height) = 0;
		virtual void Render (Module::Camera * camera) = 0;

	protected:
		SceneGraph   * pGraph;
		AssetManager * pAssetManager;

		ShaderID DefaultShader () const;

	private:
		Scene * mScene;
	};
}