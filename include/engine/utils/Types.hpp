#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace Engine::Data {
	struct Mesh;
	struct Vertex;
	struct Instance;
	struct Geometry;
	struct Material;

	class Shader;
	class Texture;

	class TextureAtlas;
}

namespace Engine::Core {
	class ObjectID;

	using GeometryID	= ObjectID;
	using MaterialID	= ObjectID;
	using ShaderID		= ObjectID;
	using TextureID		= ObjectID;
	
	using InstanceID	= ObjectID;
	using MeshID		= ObjectID;

	class DrawCommand;

	class Scene;

	struct SceneNode;
	class  SceneGraph;

	class AssetManager;
}

namespace Engine::Module {
	class Camera;
	class InstanceRenderer;
}
