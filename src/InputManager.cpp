#include <InputManager.hpp>

void InputManager::init(GLFWwindow* window) {
    m_window = window;

    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
}

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* instance = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (key >= 0 && key < GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) instance->m_keys[key] = true;
        else if (action == GLFW_RELEASE) instance->m_keys[key] = false;
    }
}

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* instance = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST) {
        if (action == GLFW_PRESS) instance->m_mouseButtons[button] = true;
        else if (action == GLFW_RELEASE) instance->m_mouseButtons[button] = false;
    }
}

void InputManager::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* instance = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    instance->m_mouseX = xpos;
    instance->m_mouseY = ypos;
}

void InputManager::update() {
    for (int i = 0; i < GLFW_KEY_LAST; ++i)
	{
        m_keysLast[i] = m_keys[i];
    }

	for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i)
	{
		m_mouseButtonsLast[i] = m_mouseButtons[i];
	}

    glfwPollEvents();
}

bool InputManager::isKeyDown(int key) const {
    return m_keys[key];
}

bool InputManager::isKeyPressed(int key) const {
    return m_keys[key] && !m_keysLast[key];
}

bool InputManager::isKeyReleased(int key) const {
    return !m_keys[key] && m_keysLast[key];
}

bool InputManager::isMouseButtonDown(int button) const
{
	return m_mouseButtons[button];
}


bool InputManager::isMouseButtonPressed(int button) const
{
	return m_mouseButtons[button] && !m_mouseButtonsLast[button];
}

bool InputManager::isMouseButtonReleased(int button) const
{
	return !m_mouseButtons[button] && m_mouseButtonsLast[button];
}


void InputManager::getMousePos(double& x, double& y) const {
    x = m_mouseX;
    y = m_mouseY;
}