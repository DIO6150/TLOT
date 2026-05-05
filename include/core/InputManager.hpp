#pragma once

#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

class InputManager {
public:
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

	void update();

	static InputManager& getInstance() {
		static InputManager instance;
		return instance;
	}

	void init(GLFWwindow* window);

	bool isKeyDown(int key) const;
	bool isKeyPressed(int key) const;
	bool isKeyReleased(int key) const;
	
	bool isMouseButtonDown(int button) const;
	bool isMouseButtonPressed(int button) const;
	bool isMouseButtonReleased(int button) const;
	void getMousePos(double& x, double& y) const;

private:
	InputManager() : m_window(nullptr), m_mouseX(0), m_mouseY(0) {}

	GLFWwindow* m_window;
	bool m_keys[GLFW_KEY_LAST];
	bool m_keysLast[GLFW_KEY_LAST];

	bool m_mouseButtons[GLFW_MOUSE_BUTTON_LAST];
	bool m_mouseButtonsLast[GLFW_MOUSE_BUTTON_LAST];
	double m_mouseX, m_mouseY;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
};
