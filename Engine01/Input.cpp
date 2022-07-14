#include "Input.h"

using namespace Input;

float Input::getVerticalAxis()
{
	//return keyWState ? 1.0f : (keySState ? -1.0f : 0.0f);
	return keyStates[GLFW_KEY_W] ? 1.0f : (keyStates[GLFW_KEY_S] ? -1.0f : 0.0f);
}

float Input::getHorizontalAxis()
{
	//return keyAState ? 1.0f : (keyDState ? -1.0f : 0.0f);
	return keyStates[GLFW_KEY_A] ? 1.0f : (keyStates[GLFW_KEY_D] ? -1.0f : 0.0f);
}

float Input::getMouseVerticalAxis()
{
	float delta = (float)(lastMouseY - mouseY);
	delta /= mouseSensitivity;
	if (delta < -1.0f) delta = -1.0f;
	if (delta > 1.0f) delta = 1.0f;
	return delta;
}

float Input::getMouseHorizontalAxis()
{
	float delta = (float)(lastMouseX - mouseX);
	delta /= mouseSensitivity;
	if (delta < -1.0f) delta = -1.0f;
	if (delta > 1.0f) delta = 1.0f;
	return delta;
}

bool Input::getKeyState(int glfwKeyCode)
{
	return keyStates[glfwKeyCode];
}

bool Input::getKeyDown(int glfwKeyCode)
{
	return keyStates[glfwKeyCode] && !keyLastStates[glfwKeyCode];
}

bool Input::getKeyUp(int glfwKeyCode)
{
	return !keyStates[glfwKeyCode] && keyLastStates[glfwKeyCode];
}

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool keyDown = action == GLFW_PRESS || action == GLFW_REPEAT;
	keyStates[key] = keyDown;
}

void Input::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX = xpos;
	mouseY = ypos;
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

}

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

void Input::update()
{
	for (int i = 0; i < GLFW_KEY_LAST + 1; i++)
	{
		keyLastStates[i] = keyStates[i];
	}
	lastMouseX = mouseX;
	lastMouseY = mouseY;
}