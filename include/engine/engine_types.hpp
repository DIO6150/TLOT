#pragma once

namespace Engine {
	class Engine;
	class Scene;

	template<class T> using Map = std::unordered_map<std::string, T>;
}

namespace Engine {
	class Batch;
	class DrawCommand;
	class InstanceData;
	class Geometry;
	class Mesh;
	struct Material;
}