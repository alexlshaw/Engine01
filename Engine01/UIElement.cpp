#include "UIElement.h"

UIElement::UIElement()
{
	visible = true;
	parent = nullptr;
	texture = nullptr;
	xPosition = 0.0f;
	yPosition = 0.0f;
	size = glm::vec2(0.0f, 0.0f);
}

UIElement::UIElement(float x, float y, glm::vec2 size, UIElement* parent, Texture* texture)
{
	//init variables
	visible = true;
	this->texture = texture;
	this->parent = parent;
	if (parent != nullptr)
	{
		parent->addChild(this);
	}
	xPosition = x;
	yPosition = y;
	this->size = size;
	hasFocus = false;

	buildVertices();
}

UIElement::~UIElement()
{
	delete mesh;
	mesh = nullptr;
}

void UIElement::buildVertices()
{
	//create the vertices	(position and scaling are handled in the shader at draw time)
	ColouredVertex v0, v1, v2, v3;
	v0.position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	v0.texCoords = glm::vec2(0, 1);
	v0.color = glm::vec4(1.0, 1.0, 1.0, 1.0);

	v1.position = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	v1.texCoords = glm::vec2(0, 0);
	v1.color = glm::vec4(1.0, 1.0, 1.0, 1.0);

	v2.position = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
	v2.texCoords = glm::vec2(1, 0);
	v2.color = glm::vec4(1.0, 1.0, 1.0, 1.0);

	v3.position = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	v3.texCoords = glm::vec2(1, 1);
	v3.color = glm::vec4(1.0, 1.0, 1.0, 1.0);

	std::vector<ColouredVertex> vertices({ v0, v1, v2, v3 });
	std::vector<unsigned int> indices({ 0, 1, 2, 0, 2, 3 });
	mesh = new Mesh(vertices, indices);
}

void UIElement::drawElement()
{
	if (texture != nullptr)
	{
		texture->use();
	}
	mesh->draw();
}

void UIElement::resize(float x, float y, glm::vec2 newSize)
{
	xPosition = x;
	yPosition = y;
	this->size = newSize;
	//now that we have changed the size, we may not be able to properly display all child elements. We should update the layout to best fit them
}

//void UIElement::handleKeypress(SDL_Event* keyboardEvent)
//{
//
//}

void UIElement::handleMouseLeftClick(float xLocation, float yLocation)
{
	//There was a left click at this location in parent coordinate space
	if (locationInElement(xLocation, yLocation))
	{
		//For a default UI element, the only thing that happens is that it gains focus
		hasFocus = true;
		float internalMouseX = xLocation - xPosition;
		float internalMouseY = yLocation - yPosition;
		//start handling clicks down the object chain
		for (unsigned int i = 0; i < children.size(); i++)
		{
			if (children.at(i)->locationInElement(internalMouseX, internalMouseY))
			{
				children.at(i)->handleMouseLeftClick(internalMouseX, internalMouseY);
			}
		}
	}
}

void UIElement::handleMouseLeftUnclick(float xLocation, float yLocation)
{
	//when handling the mouse being released, we don't care about position, just focus
	for (unsigned int i = 0; i < children.size(); i++)
	{
		float internalMouseX = xLocation - xPosition;
		float internalMouseY = yLocation - yPosition;
		if (children.at(i)->hasFocus)
		{
			children.at(i)->handleMouseLeftUnclick(internalMouseX, internalMouseY);
		}
	}
}

bool UIElement::locationInElement(float x, float y)
{
	if (x >= xPosition && y >= yPosition && x < xPosition + size.x && y < yPosition + size.y)
	{
		return true;
	}
	return false;
}

glm::vec2 UIElement::getLocation()
{
	return glm::vec2(xPosition, yPosition);
}

glm::vec2 UIElement::getAbsoluteLocation()
{
	//if it has a parent, learn the absolute position of the parent and go from there, otherwise its position is the same as its absolute position
	if (parent != nullptr)
	{
		glm::vec2 parentAbsolute = parent->getAbsoluteLocation();
		return glm::vec2(parentAbsolute.x + xPosition, parentAbsolute.y + yPosition);
	}
	else
	{
		return glm::vec2(xPosition, yPosition);
	}
}

void UIElement::addChild(UIElement* childElement)
{
	if (childElement != nullptr)
	{
		children.push_back(childElement);
	}
}

glm::vec2 UIElement::getSize()
{
	return size;
}