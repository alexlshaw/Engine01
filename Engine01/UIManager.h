#ifndef ENGINE01_UIMANAGER_H
#define ENGINE01_UIMANAGER_H

#include <vector>
#include "glad/glad.h"
#include "glm\glm.hpp"

#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include "Shader.h"
#include "Texture.h"
#include "UIElement.h"


class UIManager
{
private:
	//might be worth storing a variable "mustConvertToOrtho", so which gets checked before drawing
	//if it is true, it means that the normal rendering is in perspective, so before drawing the 2D
	//UI elements, it should convert the screen to orthographic projection
	//should probably have an overload of drawInterface that doesn't take a matrix, and instead
	//generates its own. Benefit of this is that I can use it for a 3D game UI
	int width, height;
	std::vector<UIElement*> topLevelElements;
	Shader* uiShader;
	GLuint ui_projection, ui_tex, ui_pos, ui_size;
	Texture* defaultTexture;
public:
	UIManager(int screenWidth, int screenHeight, GraphicsResourceManager* resourceManager);
	~UIManager();
	void drawInterface(glm::mat4 projectionMatrix);
	void drawInterfaceRec(glm::mat4 projectionMatrix, UIElement* currentElement);
	//void handleMouseInput(SDL_Event* mouseEvent);
	//void handleKeyboardInput(SDL_Event* keyboardEvent);
	void addElement(UIElement* element);
	void screenResize(int newWidth, int newHeight);
};


#endif