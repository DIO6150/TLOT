
#include <iostream>
#include <ctime>

#include <engine.hpp>

using namespace Engine;

namespace TestLifeCycle {
	static bool __attribute ((unused)) test1 (long int sample_size) {
		Engine::Engine engine;

		std::vector<Engine::Mesh *> meshs;
		meshs.resize (sample_size);
		std::clock_t dt;


		Engine::Scene * scene = engine.createScene ();

		auto geometry = engine.loadGeometry ("tralala", "tralala.geo");
		auto material = engine.createMaterial ("mat", engine.loadShader ("default", "assets/shaders/default.vertex", "assets/shaders/default.fragment"), nullptr);
		
		dt = std::clock ();
		for (long int i = 0; i < sample_size; ++i) {
			meshs[i] = scene->createMesh (geometry, material);
		}
		dt = std::clock () - dt;
		std::cout << "Creating " << sample_size << " Mesh(es) took: " << dt * 1000 / CLOCKS_PER_SEC << "ms \n";

		scene->printStats ();

		dt = std::clock ();
		for (long int i = 0; i < sample_size; ++i) {
			scene->removeMesh (meshs[i]);
		}
		dt = std::clock () - dt;
		std::cout << "Destroying " << sample_size << " Mesh(es) took: " << dt * 1000 / CLOCKS_PER_SEC << "ms \n";

		scene->printStats ();

		return (true);
	}

	static bool __attribute ((unused)) test2 (long int max_sample_size) {
		for (int i = 1; i < max_sample_size; ++i) {
			printf ("========================================================%d / %ld========================================================\n", i + 1, max_sample_size);
			test1 (powl (10, i));
		}

		return (true);
	}
}