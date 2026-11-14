
#include <string>
#include <map>
#include <set>

#include <TetoEngine/Application/Application.hpp>

#include <TetoEngine/Common/Transform.hpp>
#include <TetoEngine/Common/Material.hpp>

#include <TetoEngine/Extensions/TetoModel.hpp>
#include <TetoEngine/Renderer/TRID.hpp>

#include <glm/glm.hpp>

using namespace TetoEngine;

class MaterialLoadingAPITest: public Application
{
	void loadLevel(std::string levelName)
	{
		unloadCurrentLevel();

		/* read level file */
		std::map<std::string, std::string> images;
		std::map<std::string, std::string> shaders;
		std::map<std::string, MaterialLayout> materials;

		std::set<ID64> loadedLevelImages_;
		std::set<ID64> loadedLevelShaders_;
		std::set<ID64> loadedLevelMaterials_;

		for (auto const & [name, path] : images)
		{
			loadedLevelImages_.emplace(loadImage(name, path));
		}

		for (auto const & [name, path] : shaders)
		{
			loadedLevelShaders_.emplace(loadShader(name, path));
		}

		for (auto const & [name, layout] : materials)
		{
			loadedLevelMaterials_.emplace(loadMaterial(name, layout));
		}
	}

	void unloadCurrentLevel()
	{
		for (auto const & handle : loadedLevelImages_)
		{
			unloadImage(handle);
		}

		for (auto const & handle : loadedLevelShaders_)
		{
			unloadShader(handle);
		}

		for (auto const & handle : loadedLevelMaterials_)
		{
			unloadMaterial(handle);
		}

		reloadGPUResources();
	}

	void init() override
	{
		loadImage("texture/test/spada/asso", "assets/images/card_asso_spada.png");
		loadImage("texture/test/back/red"  , "assets/images/card_back_red.png");
		loadImage("texture/test/outline"   , "assets/images/card_outline.png");

		loadShader("shader/test/vertex"  , "assets/shaders/test.vertex");
		loadShader("shader/test/fragment", "assets/shaders/test.fragment");

		loadMaterial("material/test",
			{
				.shader = {"shader/test/vertex", "shader/test/fragment"},
				.shaderProperties = {
					{"u_time", UniformType::Float}
				},
				.instanceProperties = {
					{"health"   , UniformType::Float},
					{"maxHealth", UniformType::Float}
				},
			}
		);

		/* ------------------------------------------- */

		/* ------------------------------------------- */
	}

private:
	std::set<ID64> loadedLevelImages_;
	std::set<ID64> loadedLevelShaders_;
	std::set<ID64> loadedLevelMaterials_;
};

int main(int argc, char ** argv)
{
	MaterialLoadingAPITest app;
	app.run();

	return 0;
}