#include "Biome.h"

Biome::Biome(int id, glm::vec3 mapColour, std::string name, float sandLine, float snowLine, float dirtThreshold, float rockThreshold)
{
	this->id = id;
	this->mapColour = mapColour;
	this->name = name;
	this->sandLine = sandLine;
	this->snowLine = snowLine;
	this->dirtThreshold = dirtThreshold;
	this->rockThreshold = rockThreshold;
}

int Biome::getID()
{
	return id;
}

glm::vec4 Biome::determineTerrainSplat(float height, float slope)
{
	float sand = 0.0f, dirt = 0.0f, grass = 0.0f, snow = 0.0f;
	if (slope < rockThreshold)	//steeper slope than this is always rock
	{
		if (height >= snowLine)	//snow line
		{
			snow = 1.0f;
		}
		else if (height <= sandLine)	//sand line
		{
			sand = 1.0f;
		}
		else					//middle elevation: grass and dirt
		{
			if (slope > dirtThreshold)	//moderately steep slopes are dirt
			{
				dirt = 1.0f;
			}
			else				//flat ground is grass
			{
				grass = 1.0f;
			}
		}
	}
	return glm::vec4(sand, dirt, grass, snow);
}

bool Biome::rockAtPoint(glm::vec4 splat, float height, float random)
{
	//general rule: rocks can appear on rock, sand, and snow
	//however, in some biomes, this leads to excess rocks, so we reduce the spawn probability
	if (id == DESERT)
	{
		return splat[SPLAT_DIRT] < 0.1f && splat[SPLAT_GRASS] < 0.1f && random < 0.25f;
	}
	if (id == ALPINE)
	{
		return splat[SPLAT_DIRT] < 0.1f && splat[SPLAT_GRASS] < 0.1f && random < 0.25f;
	}
	return splat[SPLAT_DIRT] < 0.1f && splat[SPLAT_GRASS] < 0.1f && random > (0.5f * rockThreshold);
}

bool Biome::treeAtPoint(glm::vec4 splat, float height, float random)
{
	//obviously, we can't have a tree if the biome has no trees
	if (treeTypes.empty())
	{
		return false;
	}
	//general rule: above the sandline, must be on snow or grass
	return height > sandLine && (splat[SPLAT_GRASS] == 1.0f || splat[SPLAT_SNOW] == 1.0f);
}

TreeSpecies* Biome::pickTreeType(float random)
{
	if (treeTypes.size() == 0)
	{
		return nullptr;
	}
	else
	{
		return treeTypes[(int)(random * treeTypes.size()) % treeTypes.size()];
	}
}

void Biome::addTreeType(TreeSpecies* treeType)
{
	treeTypes.push_back(treeType);
}