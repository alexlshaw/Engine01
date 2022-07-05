#include "BuildNodeTask.h"

BuildNodeTask::BuildNodeTask(int nodeIndex, Node* nodeToBuild, WorldInfo* info)
{
	index = nodeIndex;
	node = nodeToBuild;
	worldInfo = info;
	startingX = (index % NODES_PER_WORLD_SIDE) * HEIGHTMAP_TEXTURE_WIDTH;
	startingZ = (index / NODES_PER_WORLD_SIDE) * HEIGHTMAP_TEXTURE_WIDTH;
	xMax = startingX + HEIGHTMAP_TEXTURE_WIDTH;
	zMax = startingZ + HEIGHTMAP_TEXTURE_WIDTH;
}

void BuildNodeTask::execute()
{
	buildHeightMap();
	buildSplatMap();
	node->buildTime = 0.0f;
	node->state = BUILT;
}

void BuildNodeTask::buildHeightMap()
{
	std::vector<float> heightMapValues;
	heightMapValues.reserve(HEIGHTMAP_TEXTURE_WIDTH * HEIGHTMAP_TEXTURE_WIDTH);
	for (int x = startingX; x < startingX + HEIGHTMAP_TEXTURE_WIDTH; x++)
	{
		for (int z = startingZ; z < startingZ + HEIGHTMAP_TEXTURE_WIDTH; z++)
		{
			float height = worldInfo->getTerrainHeightAtPoint((float)x, (float)z);	//TODO: we end up converting these values back and forth from float to int quite often, should probably refine this
			heightMapValues.push_back(height);
		}
	}
	node->heightMapData = heightMapValues;
}

void BuildNodeTask::buildSplatMap()
{
	int startingX = (index % NODES_PER_WORLD_SIDE) * HEIGHTMAP_TEXTURE_WIDTH;
	int startingZ = (index / NODES_PER_WORLD_SIDE) * HEIGHTMAP_TEXTURE_WIDTH;
	std::vector<glm::vec4> splatMapValues;
	splatMapValues.reserve(HEIGHTMAP_TEXTURE_WIDTH * HEIGHTMAP_TEXTURE_WIDTH);
	for (int x = startingX; x < xMax; x++)
	{
		for (int z = startingZ; z < zMax; z++)
		{
			//determine the biome to which this location belongs
			int offset = (int)(15.0f * worldInfo->noiseMapLookup(x, z) - 7.0f); //offset between -3 and 3 units away
			int offsetX = x + offset;
			int offsetZ = z + offset;
			int geoX = (int)(offsetX / FWORLD_POINT_SEPARATION);
			int geoZ = (int)(offsetZ / FWORLD_POINT_SEPARATION);
			GeoPoint geo = worldInfo->geoAtIndex(geoX, geoZ);
			Biome* biome = geo.voronoiCell->biome;
			//determine height of this point and steepest slope to a neighbouring point
			int xIndex = x % HEIGHTMAP_TEXTURE_WIDTH;
			int zIndex = z % HEIGHTMAP_TEXTURE_WIDTH;
			float height = node->heightMapData[zIndex + HEIGHTMAP_TEXTURE_WIDTH * xIndex];
			float slope = determineSlope(x, z, height);
			//use these values to determine what terrain type is drawn on this point
			glm::vec4 splat = biome->determineTerrainSplat(height, slope);
			splatMapValues.push_back(splat);
		}
	}
	node->splatMapData = splatMapValues;
}

float BuildNodeTask::determineSlope(int x, int z, float height)
{
	float x1 = fabs(height - getHeight(x + 1, z));
	float x2 = fabs(height - getHeight(x - 1, z));
	float xMax = x1 > x2 ? x1 : x2;

	float y1 = fabs(height - getHeight(x, z + 1));
	float y2 = fabs(height - getHeight(x, z - 1));
	float yMax = y1 > y2 ? y1 : y2;

	float d1 = fabs(height - getHeight(x + 1, z + 1));
	float d2 = fabs(height - getHeight(x - 1, z + 1));
	float d3 = fabs(height - getHeight(x + 1, z - 1));
	float d4 = fabs(height - getHeight(x - 1, z - 1));
	float dm1 = d1 > d2 ? d1 : d2;
	float dm2 = d3 > d4 ? d3 : d4;

	float dMax = dm1 > dm2 ? dm1 : dm2;
	float sMax = xMax > yMax ? xMax : yMax;
	return dMax > sMax ? dMax : sMax;
}

//IMPORTANT NOTE: Only call this method after building the heightmap data
float BuildNodeTask::getHeight(int x, int z)
{
	int xIndex = x % HEIGHTMAP_TEXTURE_WIDTH;
	int zIndex = z % HEIGHTMAP_TEXTURE_WIDTH;
	if (x >= startingX && x < xMax)
	{
		if (z >= startingZ && z < zMax)
		{
			return node->heightMapData[zIndex + HEIGHTMAP_TEXTURE_WIDTH * xIndex];
		}
	}
	return worldInfo->getTerrainHeightAtPoint((float)x, (float)z);
}