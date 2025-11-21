#include <engine.hpp>

Engine::Engine::Engine () {

}

Engine::Handle Engine::Engine::loadShader (__attribute__ ((unused)) const std::string url, __attribute__ ((unused)) const ED::ShaderType type) {
	// TODO: fix shader import
	return (m_shader.create ());
}

Engine::Handle Engine::Engine::loadGeometry (__attribute__ ((unused)) const std::string url) {
	// TODO: fix geometry import
	return (m_geometry.create ());
}

Engine::Handle Engine::Engine::createMaterial () {
	// TODO: fix create material
	return (m_material.create ());
}

Engine::Scene * Engine::Engine::createScene () {
	auto & scene = m_scenes.emplace_back (m_geometry);
	return (&scene);
}