#include <RenderableObject.hpp>

using namespace TLOT;

CommonObject::CommonObject(Renderer * renderer, ResourceHandle geometry, ResourceHandle materialTemplateHandle)
	: m_material{AssetManager::CreateMaterial(materialTemplateHandle)}
	, m_renderer{renderer}
{
	m_instance = renderer->Instanciate(geometry, m_material, m_transform);
}

glm::mat4 CommonObject::GetTransformMatrix() const
{
	return m_transform.GetModelMatrix();
}

glm::vec3 CommonObject::GetPosition() const
{
	return m_transform.position;
}

glm::quat CommonObject::GetRotation() const
{
	return m_transform.rotation;
}

glm::vec3 CommonObject::GetScale() const
{
	return m_transform.scale;
}

void CommonObject::SetPosition(glm::vec3 position)
{
	m_transformUpdateFlag = true;
	m_transform.position = position;
}

void CommonObject::SetPivot(glm::vec3 pivot)
{
	m_transformUpdateFlag = true;
	m_transform.pivot = pivot;
}

void CommonObject::SetRotation(glm::quat rotation)
{
	m_transformUpdateFlag = true;
	m_transform.rotation = rotation;
}

void CommonObject::SetRotation(glm::vec3 eulerAngle)
{
	m_transformUpdateFlag = true;
	glm::quat finalRot = glm::quat(eulerAngle);
	m_transform.rotation = finalRot;
}

void CommonObject::SetScale(glm::vec3 scale)
{
	m_transformUpdateFlag = true;
	m_transform.scale = scale;
}

void CommonObject::Translate(glm::vec3 translate)
{
	m_transformUpdateFlag = true;
	m_transform.position += translate;
}

void CommonObject::Rotate(glm::quat rotation)
{
	m_transformUpdateFlag = true;
	m_transform.rotation = rotation * m_transform.rotation;
}

void CommonObject::Scale(glm::vec3 scale)
{
	m_transformUpdateFlag = true;
	m_transform.scale += scale;
}

void CommonObject::Render()
{
	if (m_transformUpdateFlag)
		m_renderer->Update(m_instance, m_transform);

	if (m_materialUpdateFlag)
		m_renderer->Update(m_instance, m_material);

	m_renderer->Render(m_instance);

	m_transformUpdateFlag = false;
	m_materialUpdateFlag = false;
}

SceneObject CommonObject::GetSceneObject()
{
	return m_instance;
}

MaterialInstance & CommonObject::GetMaterial()
{
	return m_material;
}

Transform & CommonObject::GetTransform()
{
	return m_transform;
}

void CommonObject::UpdateTransform()
{
	m_transformUpdateFlag = true;
}

void CommonObject::UpdateMaterial()
{
	m_materialUpdateFlag = true;
}

void CommonObject::Destroy()
{
	m_renderer->KillInstance(m_instance);
}

#include <Internal/Style/StyleLexer.hpp>

TextObject::TextObject(
	Renderer * renderer,
	ResourceHandle normal,
	ResourceHandle italic,
	ResourceHandle bold
)
: CommonObject {renderer, AssetManager::Cache("geometry_quad"), AssetManager::Cache("material_glyph")} // très très brouillon, mais pas le temps so heh
{
	m_normalFont = normal;
	m_italicFont = italic;
	m_boldFont   = bold;

	std::cout << "TextObject generated with material index = " << m_instance.materialIndex << "\n";
}

void TextObject::SetText(std::string text, size_t fontSize)
{
	//CommonObject::SetPosition({400.0, 400.0, 0.0});

	m_text = text;
	m_maxHeight = 0;
	m_maxWidth = 0;

	for (auto & glyph : m_renderedGlyph)
	{
		glyph.Destroy();
	}
	m_renderedGlyph.clear();
	m_glyphs.clear();

	auto tokens = TokenizeString(text);

	auto const transform = GetTransform();
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

	 const std::map<std::string, ResourceHandle> style = {
		{"NORMAL", m_normalFont},
		{"ITALIC", m_italicFont},
		{"BOLD"  , m_boldFont  },
	};

	static const std::map<std::string, TagType> prefixType = {
		{"C", TextColor},
		{"X", BackgroundColor},
		{"S", Style},
	};

	glm::vec3 currentColor           = color.at("BLACK");
	glm::vec3 currentBackgroundColor = color.at("NONE"); // implement that after
	ResourceHandle currentFont       = style.at("NORMAL");
	for (auto & token : tokens)
	{
		if (token.type == TokenType::TAG)
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

		else if (token.type == TokenType::TEXT)
		{
			for (auto & character : token.value)
			{
				auto compiledToken = CommonObject {m_renderer, AssetManager::Cache("geometry_quad"), AssetManager::Cache("material_glyph")};
				compiledToken.GetMaterial().Set("glyph", m_renderer->Glyph(currentFont, character, fontSize));
				compiledToken.GetMaterial().Set("color", currentColor);

				auto font = AssetManager::GetFont(currentFont);
				auto glyph = font.Get().getGlyph(character, fontSize);

				if (!font)
				{
					Logger::panic("oh oh");
					continue;
				}
				// calculate glyph position
				float posX = lineX + static_cast<float>(glyph.bearingX);
				float posY = lineY - static_cast<float>(glyph.height - glyph.bearingY);
				//float posY = lineY + static_cast<float>(glyph.bearingY);

				compiledToken.SetPosition({posX, posY, 1.0});
				compiledToken.SetScale({glyph.width, glyph.height, 1.0});

				m_renderedGlyph.emplace_back(std::move(compiledToken));
				m_glyphs.emplace_back(glyph);
				lineX += static_cast<float>((glyph.advance) >> 6);
				m_maxWidth += static_cast<float>((glyph.advance) >> 6);
				m_maxHeight = std::max(m_maxHeight, static_cast<float>(glyph.height));
				//lineX += static_cast<float>(glyph.width);
			}
		}
	}

	std::cout << "Last TextObject character material index is : " << m_renderedGlyph.back().m_instance.materialIndex  << "\n";
}

void TextObject::Destroy()
{
	CommonObject::Destroy();
	for (auto & glyph : m_renderedGlyph)
	{
		glyph.Destroy();
	}
}

void TextObject::Render()
{
	for (auto & glyph : m_renderedGlyph)
	{
		glyph.Render();
	}
}

void TextObject::SetPosition(glm::vec3 translate)
{
	CommonObject::SetPosition(translate);

	auto const transform = GetTransform();
	float lineX = transform.position.x;
	float lineY = transform.position.y;

	int index = 0;
	for (auto & renderedGlyph : m_renderedGlyph)
	{
		auto glyph = m_glyphs[index];

		float posX = lineX + static_cast<float>(glyph.bearingX);
		float posY = lineY - static_cast<float>(glyph.height - glyph.bearingY);

		glm::vec3 newPos = glm::vec3(posX, posY, translate.z);
		renderedGlyph.SetPosition(newPos);
		renderedGlyph.SetScale({glyph.width, glyph.height, 1.0});
		index++;
		lineX += static_cast<float>((glyph.advance) >> 6);
	}
}