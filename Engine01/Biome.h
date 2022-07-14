#ifndef ENGINE01_BIOME_H
#define ENGINE01_BIOME_H

#include "glm\glm.hpp"
#include <string>
#include <vector>

#include "Settings.h"
#include "TreeSpecies.h"

const enum biomes {TUNDRA = 0, DESERT, GRASSLAND, TAIGA, SWAMP, FOREST, JUNGLE, SAVANNAH, BADLAND, MARSH, ALPINE, ASHLAND};

class Biome
{
private:
	int id;
	std::vector<TreeSpecies*> treeTypes;
public:
	Biome(int id, glm::vec3 mapColour, std::string name, float sandLine, float snowLine, float dirtThreshold, float rockThreshold);
	int getID();
	glm::vec3 mapColour;
	std::string name;
	glm::vec4 determineTerrainSplat(float height, float slope);
	bool rockAtPoint(glm::vec4 splat, float height, float random);
	bool treeAtPoint(glm:: vec4 splat, float height, float random);
	TreeSpecies* pickTreeType(float random);
	void addTreeType(TreeSpecies* treeType);
	float sandLine;
	float snowLine;
	float dirtThreshold;
	float rockThreshold;
};

#endif