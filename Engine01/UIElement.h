#ifndef ENGINE01_UIELEMENT_H
#define ENGINE01_UIELEMENT_H

#include "Mesh.h"
#include "Texture.h"

class UIElement
{
protected:
	float xPosition, yPosition;
	glm::vec2 size;	//width, height
	UIElement *parent;
	Mesh* mesh;
	Texture* texture;
	void buildVertices();		//Generates the openGL data to display the element
public:
	bool hasFocus;
	bool visible;
	UIElement();
	UIElement(float x, float y, glm::vec2 size, UIElement* parent, Texture* texture = nullptr);
	~UIElement();
	virtual void drawElement();
	virtual void resize(float x, float y, glm::vec2 newSize);
	virtual void handleMouseLeftClick(float xLocation, float yLocation);
	virtual void handleMouseLeftUnclick(float xLocation, float yLocation);
	//virtual void handleKeypress(SDL_Event* keyboardEvent);
	void addChild(UIElement* childElement);
	bool locationInElement(float x, float y);	//Determines if the specified location falls within the element (in terms of its parent's coordinate space)
	glm::vec2 getLocation();					//gets location of the element as it sees it (in terms of its parent's coordinate space)
	glm::vec2 getAbsoluteLocation();			//Gets location of the element in terms of screen coordinates
	glm::vec2 getSize();
	std::vector<UIElement*> children;
};

#endif