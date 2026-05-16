#include <Renderer/Renderables/ModelFactory.hpp>

#include <AssetManager.hpp>

using namespace TLOT;

Model ModelFactory::CreateModel (IRenderer & renderer, Mesh const & mesh, Transform transform)
{
	GeometryID geometryID = renderer.RegisterGeometry (mesh);
	InstanceID instanceID = renderer.RegisterInstance (geometryID, transform, mesh.material);

	Model model;
	model.renderer  = &renderer;
	model.geometry  = geometryID;
	model.instance  = instanceID;
	model.transform = transform;
	model.material  = mesh.material;

	return model;
}

Model ModelFactory::CreateModel (IRenderer & renderer, Mesh const & mesh)
{
	return CreateModel (renderer, mesh, {});
}

Sprite ModelFactory::CreateSprite (IRenderer & renderer)
{
	Mesh const & quad = AssetManager::GetQuadMesh ();
	Sprite sprite {CreateModel (renderer, quad)};

	return sprite;
}
