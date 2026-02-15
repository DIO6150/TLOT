
#include <engine/core/AssetManager.hpp>
#include <engine/core/Scene.hpp>
#include <engine/core/SceneGraph.hpp>
#include <engine/core/Renderer.hpp>

using namespace Engine::Core;
using namespace Engine::Data;

IRenderer::IRenderer (Scene * scene):
	pGraph		{&scene->graph},
	pAssetManager	{&scene->asset_manager},
	mScene		{scene}
	{
		
}

IRenderer::~IRenderer () {

}

ShaderID IRenderer::DefaultShader () const {
	return (mScene->defaultShader);
}
