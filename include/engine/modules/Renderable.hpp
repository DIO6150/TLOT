#pragma once

#include <engine/core/Renderer.hpp>
#include <engine/core/ResourceManager.hpp>

#include <engine/data/InstancedData.hpp>

namespace Engine::Module {
	class Renderable {
	public:
		Renderable ():
			mRenderer {nullptr}
			{

		}

		void Render () {
			mRenderer->PushModel (mModel, mShader);
		}

	protected:
		IRenderer & GetRenderer () const {
			return *mRenderer;
		}

		void SetRenderer (IRenderer & renderer) {
			mRenderer = &renderer;
		}

		const HandleID & GetShader () {
			return (mShader);
		}

		void SetShader (HandleID shader) {
			mShader = shader;
		}

		Instance<Model> & GetModel () {
			return (mModel);
		}

		void SetModel (const HandleID & model) {
			mModel = InstanceFactory<Model>::CreateInstance (model);
		}

	private:
		IRenderer * mRenderer;

		Instance<Model> mModel;
		HandleID mShader;
	};
}