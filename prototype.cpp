
#include <set>
#include <vector>
#include <unordered_map>
#include <map>
#include <stdint.h>
#include <optional>
#include <type_traits>
#include <stdexcept>

#include <glm/glm.hpp>

#include <engine/core/ResourceManager.hpp>
#include <engine/data/Texture.hpp>
#include <engine/core/Instancer.hpp>
#include <engine/data/InstancedData.hpp>

using namespace Engine::Core;
using namespace Engine::Data;
using namespace Engine::Module;

// Engine Modules


// User Objects
// Tests



int main () {
	// TODO : AssetManager should be a singleton
	// TODO : Renderer should be a singleton too, or maybe not hmmm
	Renderer renderer {};

	// loading stuff

	AssetManager::LoadShader ("default", "path/to/shader.vertex", "path/to/shader.fragment");

	const CardColor colors[] = {CardColor::CLUB, CardColor::CUP, CardColor::MONEY, CardColor::SWORD};
	const CardValue nums  [] = {
		CardValue::ACE, CardValue::KING, CardValue::QUEEN, CardValue::JACK ,
		CardValue::TEN, CardValue::NINE, CardValue::EIGHT, CardValue::SEVEN,
		CardValue::SIX, CardValue::FIVE, CardValue::FOUR,  CardValue::THREE,
		CardValue::TWO
	};

	for (const auto & color : colors) {
		for (const auto & num : nums) {
			std::string name = color.ToString () + "_" + num.ToString ();
			std::string path = "path/to/asset/textures/" + name + ".png";
			AssetManager::LoadTexture (name, path);
		}
	}

	// creating a card

	Card jimbo {CardColor::CLUB, 10};
	jimbo.ChangeColor (CardColor::MONEY);

	while (true) {
		jimbo.Render ();

		renderer.Render ();
		renderer.Clear ();
	}

	return (1);
}