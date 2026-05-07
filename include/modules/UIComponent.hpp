#pragma once

#include <modules/Sprite.hpp>

namespace TLOT
{

	class UIComponent
	{
	public:
		~UIComponent ()
		{
			for (auto & [key, sprite] : m_sprites)
			{
				sprite.Kill ();
			}

			for (auto & child : m_children)
			{
				child->m_parent = nullptr;
			}
		}

		void SetParent (UIComponent & parent)
		{
			m_parent = &parent;
			m_parent->AddChild (*this);
		}

		void AddChild (UIComponent & child)
		{
			m_children.emplace (&child);
			child.SetParent (*this);
		}

		glm::vec3 GetEffectivePosition ()
		{
			return GetPosition () + glm::vec3 {pivot, 0.0};
		}

		glm::vec3 GetPosition ()
		{
			if (m_parent)
			{
				return position + m_parent->GetPosition ();
			}

			return position;
		}

	protected:

		auto const & Children () const { return m_children; }
		auto Parent () { return m_parent; }

		glm::vec3 position; // x, y, z-index
		glm::vec2 pivot; // x, y

	private:
		virtual std::multimap<std::string, std::string> const RequestTextures () const = 0;
		
		std::map<std::string, Sprite> m_sprites;

		std::set<UIComponent *> m_children;
		UIComponent * m_parent = nullptr;
	};
}
