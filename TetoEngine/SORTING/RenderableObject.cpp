
#include <TetoEngine/Rendering/RenderableObject/RenderableObject.hpp>
#include <TetoEngine/Utils/Logger.hpp>

#include <cmath>


using namespace TetoEngine;

CommonObject::CommonObject(Renderer * renderer, ID64 geometry, ID64 materialTemplateHandle)
	: material_{AssetManager::CreateMaterial(materialTemplateHandle)}
	, renderer_{renderer}
{
	instance_ = renderer->instanciate(geometry, material_, transform_);
}

glm::mat4 CommonObject::matrix() const
{
	return transform_.getModelMatrix();
}

glm::vec3 CommonObject::position() const
{
	return transform_.position;
}

glm::quat CommonObject::rotation() const
{
	return transform_.rotation;
}

glm::vec3 CommonObject::scale() const
{
	return transform_.scale;
}

void CommonObject::setPosition(glm::vec3 position)
{
	dirtyTransform_ = true;
	transform_.position = position;
}

void CommonObject::setPivot(glm::vec3 pivot)
{
	dirtyTransform_ = true;
	transform_.pivot = pivot;
}

void CommonObject::setRotation(glm::quat rotation)
{
	dirtyTransform_ = true;
	transform_.rotation = rotation;
}

void CommonObject::setRotation(glm::vec3 eulerAngle)
{
	dirtyTransform_ = true;
	glm::quat finalRot = glm::quat(eulerAngle);
	transform_.rotation = finalRot;
}

void CommonObject::setScale(glm::vec3 scale)
{
	dirtyTransform_ = true;
	transform_.scale = scale;
}

void CommonObject::translate(glm::vec3 translate)
{
	dirtyTransform_ = true;
	transform_.position += translate;
}

void CommonObject::rotate(glm::quat rotation)
{
	dirtyTransform_ = true;
	transform_.rotation = rotation * transform_.rotation;
}

void CommonObject::scale(glm::vec3 scale)
{
	dirtyTransform_ = true;
	transform_.scale += scale;
}

void CommonObject::render()
{
	if (dirtyTransform_)
		renderer_->Update(instance_, transform_);

	if (dirtyMaterial_)
		renderer_->Update(instance_, material_);

	renderer_->render(instance_);

	dirtyTransform_ = false;
	dirtyMaterial_ = false;
}

SceneObject CommonObject::get()
{
	return instance_;
}

MaterialInstance & CommonObject::material()
{
	return material_;
}

Transform & CommonObject::transform()
{
	return transform_;
}

void CommonObject::updateTransform()
{
	dirtyTransform_ = true;
}

void CommonObject::updateMaterial()
{
	dirtyMaterial_ = true;
}

void CommonObject::destroy()
{
	renderer_->killInstance(instance_);
}

#include <TetoEngine/Private/Renderer/StyleLexer.hpp>

TextObject::TextObject(
	Renderer * renderer,
	ID64 normal,
	ID64 italic,
	ID64 bold
)
: CommonObject {renderer, AssetManager::Cache("geometry_quad"), AssetManager::Cache("material_glyph")} // très très brouillon, mais pas le temps so heh
{
	normalFont_ = normal;
	italicFont_ = italic;
	boldFont_   = bold;
}

void TextObject::setText(std::string text, size_t fontSize)
{
	//CommonObject::setPosition({400.0, 400.0, 0.0});

	textString_ = text;
	maxHeight_ = 0;
	maxWidth_ = 0;

	for (auto & glyph : renderedGlyphList_)
	{
		glyph.destroy();
	}
	renderedGlyphList_.clear();
	glyphList_.clear();

	if (text.empty())
		return;

	auto tokens = TokenizeString(text);

	auto const transform = transform();
	float lineX = transform.position.x;
	float lineY = transform.position.y;
	
	enum TagType
	{
		TextColor,
		BackgroundColor,
		Style
	};

	static const std::map<std::string, glm::vec4> color = {
		{"ORANGE", {0.902, 0.627, 0.192, 1.0}},
		{"RED"   , {0.812,     0,     0, 1.0}},
		{"YELLOW", {    1, 0.894, 0.475, 1.0}},
		{"BLUE"  , {0.157, 0.553, 0.941, 1.0}},
		{"GREEN" , {0.137, 0.769, 0.184, 1.0}},
		{"GREY"  , { 0.78,  0.78,  0.78, 1.0}},
		{"WHITE" , {    1,     1,     1, 1.0}},
		{"BLACK" , {    0,     0,     0, 1.0}},
		{"NONE"  , {  0.0,   0.0,   0.0, 0.0}}
	};

	 const std::map<std::string, ID64> style = {
		{"NORMAL", normalFont_},
		{"ITALIC", italicFont_},
		{"BOLD"  , boldFont_  },
	};

	static const std::map<std::string, TagType> prefixType = {
		{"C", TextColor},
		{"X", BackgroundColor},
		{"S", Style},
	};

	glm::vec3 currentColor           = color.at("BLACK");
	glm::vec3 currentBackgroundColor = color.at("NONE"); // implement that after
	ID64 currentFont       = style.at("NORMAL");
	for (auto & token : tokens)
	{
		if (token.type_ == TokenType::TAG)
		{
			currentColor           = color.at("BLACK");
			currentBackgroundColor = color.at("NONE"); // implement that after
			currentFont            = style.at("NORMAL");

			if (token.value.size() == 0)
				continue;

			auto tokenSplit = SplitString(token.value, ',');

			if (tokenSplit.size() < 1)
				continue;

			for (auto & tagPair : tokenSplit)
			{
				auto tagSplit = SplitString(tagPair, ':');

				if (tagSplit.size() < 2)
					continue;
				
				std::string tagPrefix = tagSplit[0];
				if (prefixType.find(tagPrefix) != prefixType.end())
				{
					TagType tagType = prefixType.at(tagPrefix);
					std::string tagValue = tagSplit[1];

					if (tagType == TextColor && color.find(tagValue) != color.end())
					{
						currentColor = color.at(tagValue);
					}
					else if (tagType == BackgroundColor && color.find(tagValue) != color.end())
					{
						currentBackgroundColor = color.at(tagValue);
					}
					else if (tagType == Style && style.find(tagValue) != style.end())
					{
						currentFont = style.at(tagValue);
					}
					else
					{
						Logger::log(LogLevel::Warning, "Invalid Tag Value [{}]", tagValue);
					}
				}
				else
				{
					Logger::log(LogLevel::Warning, "Invalid Tag Prefix [{}]", tagPrefix);
				}
			}
		}

		else if (token.type_ == TokenType::TEXT)
		{
			if (token.value.empty())
				continue;

			for (auto & character : token.value)
			{
				auto compiledToken = CommonObject {renderer_, AssetManager::Cache("geometry_quad"), AssetManager::Cache("material_glyph")};
				compiledToken.material().setUniform("glyph", renderer_->Glyph(currentFont, character, fontSize));
				compiledToken.material().setUniform("color", currentColor);

				auto font = AssetManager::GetFont(currentFont);
				auto glyph = font.get().getGlyph(character, fontSize);

				if (!font)
				{
					Logger::panic("oh oh");
					continue;
				}
				// calculate glyph position
				float posX = lineX + static_cast<float>(glyph.bearingX_);
				float posY = lineY - static_cast<float>(glyph.height - glyph.bearingY_);
				//float posY = lineY + static_cast<float>(glyph.bearingY_);

				compiledToken.setPosition({posX, posY, 1.0});
				compiledToken.setScale({glyph.width_, glyph.height, 1.0});

				renderedGlyphList_.emplace_back(std::move(compiledToken));
				glyphList_.emplace_back(glyph);
				lineX += static_cast<float>((glyph.advance_) >> 6);
				maxWidth_ += static_cast<float>((glyph.advance_) >> 6);
				maxHeight_ = std::max(maxHeight_, static_cast<float>(glyph.height));
				//lineX += static_cast<float>(glyph.width_);
			}
		}
	}
}

void TextObject::destroy()
{
	CommonObject::destroy();
	for (auto & glyph : renderedGlyphList_)
	{
		glyph.destroy();
	}
}

void TextObject::render()
{
	for (auto & glyph : renderedGlyphList_)
	{
		glyph.render();
	}
}

void TextObject::setPosition(glm::vec3 translate)
{
	CommonObject::setPosition(translate);

	auto const transform = transform();
	float lineX = transform.position.x;
	float lineY = transform.position.y;

	int index = 0;
	for (auto & renderedGlyph : renderedGlyphList_)
	{
		auto glyph = glyphList_[index];

		float posX = lineX + static_cast<float>(glyph.bearingX_);
		float posY = lineY - static_cast<float>(glyph.height - glyph.bearingY_);

		glm::vec3 newPos = glm::vec3(posX, posY, translate.z);
		renderedGlyph.setPosition(newPos);
		renderedGlyph.setScale({glyph.width_, glyph.height, 1.0});
		index++;
		lineX += static_cast<float>((glyph.advance_) >> 6);
	}
}