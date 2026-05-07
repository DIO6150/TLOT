#include <modules/UIComponent.hpp>
#include <modules/UIComponentManager.hpp>

namespace TLOT
{
	class IButtonCallback
	{
	public:
		virtual void OnResize () = 0;
		virtual void OnHover () = 0;
		virtual void OnPress () = 0;
	};

	class UIButton : UIComponent
	{
	public:
		UIButton (glm::vec3 defaultColor, glm::vec3 hoverColor, glm::vec3 pressedColor, IButtonCallback & callback):
			m_defaultColor {defaultColor}, m_hoverColor {hoverColor}, m_pressedColor {pressedColor}, m_callback {callback}
		{
			
		}

		void OnResize ()
		{
			m_callback.OnResize ();
		}

		void OnHover ()
		{
			m_callback.OnHover ();
		}

		void OnPressed ()
		{
			m_callback.OnPress ();
		}

		void SetAnchor (glm::vec2 anchor)
		{
			pivot = anchor;
		}

	private:
		friend class UIComponentManager;

		glm::vec3 m_defaultColor = glm::vec3 {1.0};
		glm::vec3 m_hoverColor   = glm::vec3 {1.0};
		glm::vec3 m_pressedColor = glm::vec3 {1.0};

		IButtonCallback & m_callback;

		inline std::multimap<std::string, std::string> const RequestTextures () const override
		{
			return {
				{"topLeft", "ui_button_corner_top_left"},
				{"topRight", "ui_button_corner_top_right"},
				{"bottomLeft", "ui_button_corner_bottom_left"},
				{"bottomRight", "ui_button_corner_bottom_right"},
				{"center", "ui_button_center"}
			};
		}
	};
}
