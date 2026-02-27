#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace Engine::Data {
	struct Mesh;
	struct Model;
	struct Vertex;
	struct Geometry;
	struct Material;

	class Shader;
	class Texture;

	class TextureAtlas;

	class ShaderInputs;
}

namespace Engine::Core {
	class HandleID;
	class DrawCommand;
	class AssetManager;
}

namespace Engine::Module {
	class Camera;
	class InstanceRenderer;
}

namespace Engine::Internal {
	class FrameBuffer;
	class RenderingGroup;

	template<class T>
	struct RenderInput;
}