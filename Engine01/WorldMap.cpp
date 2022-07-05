#include "WorldMap.h"

WorldMap::WorldMap(float x, float y, glm::vec2 size, UIElement* parent, Texture* defaultTexture, Texture* biomeTexture) : UIElement(x, y, size, parent)
{
	activeMapType = 0;
	visible = false;
	mapTextures.reserve(MAP_TYPE_COUNT);
	mapTextures.push_back(defaultTexture);
	mapTextures.push_back(biomeTexture);
}

WorldMap::~WorldMap()
{
	//the map textures are not handled by the graphics resource manager, therefore the worldmap must handle deletion of its own textures
	mapTextures.clear();
}

void WorldMap::cycleActiveMapType()
{
	activeMapType = ++activeMapType % MAP_TYPE_COUNT;
}

void WorldMap::drawElement()
{
	if (visible)
	{
		mapTextures[activeMapType]->use();
		mesh->draw();
	}
}