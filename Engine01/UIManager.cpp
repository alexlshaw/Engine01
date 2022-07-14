#include "UIManager.h"

UIManager::UIManager(int screenWidth, int screenHeight, GraphicsResourceManager* resourceManager)
{
	width = screenWidth;
	height = screenHeight;
	//load default ui shader
	uiShader = resourceManager->loadShader("ui/UIElement");
	ui_projection = uiShader->getUniformLocation("orthoMatrix");
	ui_tex = uiShader->getUniformLocation("tex");
	ui_pos = uiShader->getUniformLocation("pos");
	ui_size = uiShader->getUniformLocation("size");
	//load default ui element texture
	defaultTexture = resourceManager->loadTexture("TestImage");
}

UIManager::~UIManager()
{
	topLevelElements.clear();
	//no need to worry about deleting shader/defaultTexture, resource manager takes care of that
}

//void UIManager::handleMouseInput(SDL_Event* mouseEvent)
//{
//	//if the event is a mousewheel motion:
//	if (mouseEvent->type == SDL_MOUSEWHEEL)
//	{
//		//Find the top level element that currently has focus
//		//pass the event to it
//	}
//	else if (mouseEvent->type == SDL_MOUSEBUTTONDOWN) //if the event is a click of some sort:
//	{
//		float xLocation = (float)mouseEvent->button.x;
//		float yLocation = (float)(height - mouseEvent->button.y);	//UI elements are defined in bottom to top coordinate space, which is the opposite of what SDL gives for click coordinates
//		//find the element that currently has focus
//		//find the element that the mouse is over
//		UIElement* currentFocus = nullptr;
//		UIElement* newTarget = nullptr;
//		for (Uint32 i = 0; i < topLevelElements.size(); i++)
//		{
//			if (topLevelElements.at(i)->hasFocus)
//			{
//				currentFocus = topLevelElements.at(i);
//			}
//			if (topLevelElements.at(i)->locationInElement(xLocation, yLocation))
//			{
//				newTarget = topLevelElements.at(i);
//			}
//		}
//		//handle a potential click outside the currently focused
//		if (currentFocus != nullptr)
//		{
//			if (newTarget == nullptr || newTarget != currentFocus)
//			{
//				//do stuff here (some elements may demand focus or be hidden when they lose focus)
//			}
//		}
//		//Call the handleMouse method for the targeted element
//		if (newTarget != nullptr)
//		{
//			newTarget->handleMouseLeftClick(xLocation, yLocation);
//		}
//	}
//	else if (mouseEvent->type == SDL_MOUSEBUTTONUP)
//	{
//		float xLocation = (float)mouseEvent->button.x;
//		float yLocation = (float)(height - mouseEvent->button.y);	//UI elements are defined in bottom to top coordinate space, which is the opposite of what SDL gives for click coordinates
//		for (Uint32 i = 0; i < topLevelElements.size(); i++)
//		{
//			if (topLevelElements.at(i)->hasFocus)
//			{
//				topLevelElements.at(i)->handleMouseLeftUnclick(xLocation, yLocation);
//			}
//		}
//	}
//}

//void UIManager::handleKeyboardInput(SDL_Event* keyboardEvent)
//{
//	//find the top level element that currently has focus
//	//call the handlekeypress method for that element. It will call for its children as required
//	for (Uint32 i = 0; i < topLevelElements.size(); i++)
//	{
//		if (topLevelElements.at(i)->hasFocus)
//		{
//			topLevelElements.at(i)->handleKeypress(keyboardEvent);
//		}
//	}
//}

void UIManager::drawInterface(glm::mat4 projectionMatrix)
{
	uiShader->use();
	uiShader->setUniform(ui_projection, projectionMatrix);
	uiShader->setUniform(ui_tex, 0);
	//iterate over all the ui elements and call their draw methods
	for (unsigned int i = 0; i < topLevelElements.size(); i++)
	{
		//texture specifications are set for every element, because individual elements may override them (in the case of texture, should override them)
		defaultTexture->use();
		drawInterfaceRec(projectionMatrix, topLevelElements[i]);
	}
}

void UIManager::drawInterfaceRec(glm::mat4 projectionMatrix, UIElement* currentElement)
{
	if (currentElement->visible)
	{
		uiShader->setUniform(ui_pos, currentElement->getAbsoluteLocation());
		uiShader->setUniform(ui_size, currentElement->getSize());
		currentElement->drawElement();
		for (auto& child : currentElement->children)
		{
			drawInterfaceRec(projectionMatrix, child);
		}
	}
}

void UIManager::addElement(UIElement* element)
{
	topLevelElements.push_back(element);
}

void UIManager::screenResize(int newWidth, int newHeight)
{
	//Need to calculate new positions and sizes for top level elements
	//Where possible, we keep UI elements the same size and just let the size of the main pane change

	//after updating the sizes of UI elements, we record the new screen size
	width = newWidth;
	height = newHeight;
}