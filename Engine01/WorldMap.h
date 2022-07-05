#ifndef ENGINE01_WORLDMAP_H
#define ENGINE01_WORLDMAP_H

#include <vector>

#include "Texture.h"
#include "UIElement.h"

enum MapType {DEFAULT = 0, BIOME = 1};
const static int MAP_TYPE_COUNT = 2;

class WorldMap : public UIElement
{
private:
	int activeMapType;
	std::vector<Texture*> mapTextures;
public:
	WorldMap();
	~WorldMap();
	WorldMap(float x, float y, glm::vec2 size, UIElement* parent, Texture* defaultTexture, Texture* biomeTexture);
	void cycleActiveMapType();
	virtual void drawElement();
};

#endif