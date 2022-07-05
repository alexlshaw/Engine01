#ifndef ENGINE01_WORLDINFO_H
#define ENGINE01_WORLDINFO_H

#include <algorithm>
#include <fstream>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Biome.h"
#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include "Math.h"
#include "Material.h"
#include "Noise.h"
#include "Rock.h"
#include "RockType.h"
#include "Settings.h"
#include "StopWatch.h"
#include "Texture.h"
#include "Tree.h"
#include "TreeSpecies.h"
#include "Voronoi.h"

class GeoPoint
{
private:
public:
	float topography;
	Site* voronoiCell;
};

//Class which contains the methods and data (climate, geography, etc) used for constructing a world
//Stores information about a world, such as the regions it contains, but does not store the world objects that are generated from this information
class WorldInfo
{
private:
	GraphicsResourceManager* resourceManager;
	float* noiseMap;
	std::vector<Biome*> biomes;
	std::map<std::string, Biome*> biomesByName;
	Biome *tundra, *desert, *grassland, *taiga, *swamp, *forest, *jungle, *savannah, *badlands, *marsh, *alpine, *ashland;
	std::vector<TreeSpecies*> treeSpecies;
	std::vector<RockType*> rockTypes;
	float lookUpHeightValue(float x, float z, float frequency);
	std::vector<std::vector<GeoPoint>> geoData;
	GeoPoint geoAtPoint(float x, float z);
	void buildGeoAndClimateData();
	Site* findVoronoiCellAtIndex(int x, int z);
	float distanceToMatchingSite(Corner* corner, std::function<bool(Site*)> predicate);
	float heightOfPointInSite(int pointX, int pointZ, Site* site);
	void initialiseBiomes();
	void initialiseTrees();	//generates the core objects, does not place references
	void initialiseRocks();	//generates the core objects, does not place references
	Biome* selectBiome(float moisture, float temperature);
	std::vector<std::string> splitString(const std::string &s, char delimiter);
public:
	WorldInfo(GraphicsResourceManager* resourceManager);
	~WorldInfo();
	float getTerrainHeightAtPoint(float x, float z);
	GeoPoint geoAtIndex(int x, int z);
	Voronoi* voro;
	//std::vector<Tree*> trees;
	std::map<TreeSpecies*, std::vector<Tree*>> trees;
	std::vector<Rock*> rocks;
	float noiseMapLookup(int x, int z);	//returns the noisemap value corresponding to a given xz coordinate
};

#endif