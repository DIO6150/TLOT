
#include <string>
#include <map>

#include <TetoEngine/Application/Application.hpp>

#include <TetoEngine/Common/Transform.hpp>
#include <TetoEngine/Common/Material.hpp>

#include <TetoEngine/Extensions/TetoModel.hpp>
#include <TetoEngine/Renderer/TRID.hpp>

#include <glm/glm.hpp>

using namespace TetoEngine;


using Suit = int;
using CardValue = int;

class Card
{
public:
	Suit      getSuit() const;
	CardValue getValue() const;
	int       getNumericValue() const;

	std::string getName() const;
	std::string getDescription() const;
	std::string getEffectsSummary() const;

	std::string getKey() const;
	static std::string getKey(Suit s, CardValue v);

	void BoostValue(int amt) { valueBoost += amt; }
private:
	std::string name;
	std::string description;

	Suit suit = 1;
	CardValue value = 1;
	int valueBoost = 0;
};

class Hoverable
{
public:
	void init(Application * app);

	enum class Type
	{
		None, Instant, Short, Medium, Long
	};

	virtual void onHoverStart();
	virtual void onHoverStop();

	Type computeType();

private:
	Application * app_;
};

class TooltipModel : public TetoModel
{
public:
	void init(Application * app)
	{
		TetoModel::init(app);
	}

	void setTitle(std::string title)
	{

	}

	void setDescription(std::string description)
	{

	}
};

// should there be virtual inheritance ?
class CardModel : public TetoModel, Hoverable
{
public:
	void init(Application * app, Card * card)
	{
		TetoModel::init(app);
		Hoverable::init(app);

		auto mesh       = pushMesh("mesh/card");
		auto & material = getMaterial(mesh.first);

		material.setTexture("front"  , getTexture(card->getKey()));
		material.setTexture("back"   , getTexture("texture/card/back"));
		material.setTexture("outline", getTexture("texture/card/outline"));
		
		cardTooltip_.init(app);
		cardTooltip_.setTitle(card->getName());
		cardTooltip_.setDescription(card->getDescription());
		cardTooltip_.setParent(this);
		cardTooltip_.hide();
		
		updateMaterial();
	}

	void onHoverStart() override
	{
		if (computeType() == Hoverable::Type::Medium)
			cardTooltip_.show();
	}

	void onHoverStop() override
	{
		cardTooltip_.hide();
	}

private:
	TooltipModel cardTooltip_;
};

class EnemyModel : public TetoModel
{
public:
	void init(Application * app, std::string name)
	{
		TetoModel::init(app);

		auto mesh       = pushMesh("mesh/enemy");
		auto & material = getMaterial(mesh.first);

		material.setTexture("sprite", getTexture("texture/enemy/" + name));

		updateMaterial();
	}
};

class Scopalatro: public Application
{
	void init() override
	{
		setTitle("Scopalatro");
		setSize(1280, 800);

		std::vector<float> vertices = {
			0.f, 0.f, 0.f,   0.f, 0.f,   0.f, 0.f, 0.f,
			1.f, 0.f, 0.f,   1.f, 0.f,   0.f, 0.f, 0.f,
			1.f, 1.f, 0.f,   1.f, 1.f,   0.f, 0.f, 0.f,
			0.f, 0.f, 0.f,   0.f, 0.f,   0.f, 0.f, 0.f,
			1.f, 1.f, 0.f,   1.f, 1.f,   0.f, 0.f, 0.f,
			0.f, 1.f, 0.f,   0.f, 1.f,   0.f, 0.f, 0.f,
		};

		std::vector<uint32_t> indices = {
			0, 1, 2, 3, 4, 5
		};

		auto quadGeometry = loadGeometry("geometry/quad", vertices, indices);

		auto badaliscImage    = loadImage("texture/enemy/badalisc" , "assets/images/enemy_badalisc.png");
		auto cardFrontImage   = loadImage("texture/card/spada/asso", "assets/images/card_asso_spada.png");
		auto cardBackImage    = loadImage("texture/card/back/red"  , "assets/images/card_back_red.png");
		auto cardOutlineImage = loadImage("texture/card/outline"   , "assets/images/card_outline.png");

		auto enemyVertex = loadShader("shader/enemy/vertex", "assets/shaders/enemy.vertex");
		auto cardVertex  = loadShader("shader/card/vertex" , "assets/shaders/card.vertex");

		auto enemyFragment = loadShader("shader/enemy/fragment", "assets/shaders/enemy.fragment");
		auto cardFragment  = loadShader("shader/card/fragment" , "assets/shaders/card.fragment");

		auto enemyMaterial = loadMaterial("material/enemy",
			{
				.uniforms_= {
					{"health"   , UniformType::Float},
					{"maxHealth", UniformType::Float}
				},
				.textures_= {
					"enemySprite"
				}
			}
		);
		auto cardMaterial = loadMaterial("material/card",
			{
				.uniforms_= {},
				.textures_= {
					"cardFront",
					"cardBack",
					"cardOutline",
				}
			}
		);

		auto enemyMesh = loadMesh("mesh/enemy", quadGeometry, cardMaterial);
		auto cardMesh  = loadMesh("mesh/card", quadGeometry, enemyMaterial);
		
		/* ------------------------------------------- */

		registerTexture(badaliscImage);
		auto enemyProgram = createProgram(enemyVertex, enemyFragment);
		auto cardProgram = createProgram(cardVertex, cardFragment);

		Technique enemyTechnique;
		enemyTechnique.material = enemyMaterial;
		enemyTechnique.program  = enemyProgram;
		registerTechnique(enemyTechnique);

		Technique cardTechnique;
		cardTechnique.material = cardMaterial;
		cardTechnique.program  = cardProgram;
		registerTechnique(enemyTechnique);

		/* ------------------------------------------- */


		Card carde;
		
		card    .init(this, &carde);
		badalisc.init(this, "badalisc");


		/* ------------------------------------------- */
	}

	void update() override
	{
		double dt = deltaTime();
		float const SPEED = 5.0f;

		card.translate({dt * SPEED, 0.0f , 0.0f});
	}

	void render() override
	{
		card.draw();
		badalisc.draw();
	}

	void clean() override
	{
		card.destroy();
		badalisc.destroy();
	}

private:
	CardModel  card;
	EnemyModel badalisc;
};

int main(int argc, char ** argv)
{
	Scopalatro game;
	game.run();

	return 0;
}