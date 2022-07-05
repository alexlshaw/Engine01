#include "WorldInfo.h"

WorldInfo::WorldInfo(GraphicsResourceManager* resourceManager)
{
	this->resourceManager = resourceManager;
	noiseMap = new float[NOISEMAP_X * NOISEMAP_Z];
	GenerateNoiseMap(noiseMap, NOISEMAP_X, NOISEMAP_Z);

	//construct the geo data
	//static CStopWatch timer;
	initialiseBiomes();
	buildGeoAndClimateData();	//contruct our voronoi diagram
	geoData.reserve(WORLD_POINTS_PER_SIDE);
	for (int x = 0; x < WORLD_POINTS_PER_SIDE; x++)
	{
		std::vector<GeoPoint> nv;
		nv.reserve(WORLD_POINTS_PER_SIDE);
		geoData.push_back(nv);
		for (int z = 0; z < WORLD_POINTS_PER_SIDE; z++)
		{
			GeoPoint p;
			Site* c = findVoronoiCellAtIndex(x, z);
			p.topography = heightOfPointInSite(x, z, c);
			p.voronoiCell = c;
			geoData[x].push_back(p);
		}
	}

	//generate game objects
	initialiseTrees();
	initialiseRocks();
	//float sec = timer.GetElapsedSeconds();
	//printf("Build time: %f\n", sec);
}

WorldInfo::~WorldInfo()
{
	//for (auto& vp : voronoiPoints)
	//{
	//	delete vp;
	//}
	if (voro != nullptr)
	{
		delete voro;
	}
	if (noiseMap != nullptr)
	{
		delete[] noiseMap;
		noiseMap = nullptr;
	}
	rocks.clear();
	for (auto& tree : trees)
	{
		tree.second.clear();
	}
	treeSpecies.clear();
	rockTypes.clear();
	biomes.clear();
}

float WorldInfo::getTerrainHeightAtPoint(float x, float z)
{
	if (x < 0.0f)
	{
		x += FNOISEMAP_X;
	}
	if (z < 0.0f)
	{
		z += FNOISEMAP_Z;
	}
	GeoPoint geoPoint = geoAtPoint(x, z);
	float topo = geoPoint.topography;
	float geo = 12.0f * lookUpHeightValue(x, z, 0.04f);
	float detail = 2.0f * lookUpHeightValue(x, z, 0.2f);
	return topo + geo + detail;
}

GeoPoint WorldInfo::geoAtPoint(float x, float z)
{
	int x1 = (int)(x / FWORLD_POINT_SEPARATION);
	int x2 = x1 + 1;
	int z1 = (int)(z / FWORLD_POINT_SEPARATION);
	int z2 = z1 + 1;
	float xMod = fmodf(x, FWORLD_POINT_SEPARATION);
	float zMod = fmodf(z, FWORLD_POINT_SEPARATION);
	float xLerpFactor = xMod == 0.0f ? 0.0f : xMod / FWORLD_POINT_SEPARATION;
	float zLerpFactor = zMod == 0.0f ? 0.0f : zMod / FWORLD_POINT_SEPARATION;
	GeoPoint g1 = geoData[x1][z1];
	GeoPoint g2 = geoData[x2][z1];
	GeoPoint g3 = geoData[x1][z2];
	GeoPoint g4 = geoData[x2][z2];
	GeoPoint output;
	float xLerp1 = fastLerp(g1.topography, g2.topography, xLerpFactor);
	float xLerp2 = fastLerp(g3.topography, g4.topography, xLerpFactor);
	output.topography = fastLerp(xLerp1, xLerp2, zLerpFactor);
	output.voronoiCell = geoData[xLerpFactor < 0.5f ? x1 : x2][zLerpFactor < 0.5f ? z1 : z2].voronoiCell;	//choose the voronoi cell of the GeoPoint
	return output;
}

GeoPoint WorldInfo::geoAtIndex(int x, int z)
{
	return geoData[x][z];
}

//We probably don't need the integer tests; as we multiply x/z by a frequency, very few x/z values will be integral
//thus the additional cost of handling it separately probably outweighs what we save on the integer values
float WorldInfo::lookUpHeightValue(float x, float z, float frequency)
{
	x *= frequency;
	z *= frequency;
	x = fmodf(x, FNOISEMAP_X);	//make sure our lookup is actually in the table
	z = fmodf(z, FNOISEMAP_Z);

	//generate our surrounding values for interpolation
	int x1 = (int)x;
	int z1 = (int)z;
	float fx1 = (float)x1;
	float fz1 = (float)z1;

	if (fx1 == x && fz1 == z)
	{
		//we know that both x and z are integers, so we can just return the table lookup
		return noiseMap[x1 + (NOISEMAP_X * z1)];
	}
	else if (fx1 == x)
	{
		//we know the x value is an integer, no need to interpolate on x
		int z2 = (z1 + 1) % NOISEMAP_Z;
		float v1 = noiseMap[x1 + (NOISEMAP_X * z1)];
		float v2 = noiseMap[x1 + (NOISEMAP_X * z2)];
		return interpolateCosine(v1, v2, z - fz1);
	}
	else if ((float)z1 == z)
	{
		//we know the z value is an integer, no need to interpolate on z
		int x2 = (x1 + 1) % NOISEMAP_X;
		float v1 = noiseMap[x1 + (NOISEMAP_X * z1)];
		float v2 = noiseMap[x2 + (NOISEMAP_X * z1)];
		return interpolateCosine(v1, v2, x - fx1);
	}
	else
	{
		//neither x nor z is an integer, thus we must interpolate in both cases
		int x2 = (x1 + 1) % NOISEMAP_X;
		int z2 = (z1 + 1) % NOISEMAP_Z;
		float v1 = noiseMap[x1 + (NOISEMAP_X * z1)];
		float v2 = noiseMap[x2 + (NOISEMAP_X * z1)];
		float v3 = noiseMap[x1 + (NOISEMAP_X * z2)];
		float v4 = noiseMap[x2 + (NOISEMAP_X * z2)];
		//determine diff values
		float xDiff = x - fx1;
		//interpolate along x
		float interpolatedX1 = interpolateCosine(v1, v2, xDiff);
		float interpolatedX2 = interpolateCosine(v3, v4, xDiff);
		//then interpolate our x-interpolated values along z
		return interpolateCosine(interpolatedX1, interpolatedX2, z - fz1);
	}
}

void WorldInfo::buildGeoAndClimateData()
{
	//step 1: Create a large selection of random points
	int pointCount = 1000;	//low point count to start with
	std::vector<glm::vec2> voroCoords;
	voroCoords.reserve(pointCount);

	//create random points
	float rX, rZ;
	for (int i = 0; i < pointCount; i++)
	{
		rX = FWORLD_MAP_TEXTURE_WIDTH * RandomFloat();
		rZ = FWORLD_MAP_TEXTURE_WIDTH * RandomFloat();
		voroCoords.push_back(glm::vec2(rX, rZ));
	}

	//Step 2: Build a voronoi graph from these points
	voro = new Voronoi(0.1f);
	voro->generateVoronoi(voroCoords, 0.0f, FWORLD_MAP_TEXTURE_WIDTH - 1.0f, 0.0f, FWORLD_MAP_TEXTURE_WIDTH - 1.0f);


	//step 3: all border nodes become water
	float xMax = voro->borderMaxX;
	float zMax = voro->borderMaxY;
	for (unsigned int i = 0; i < voro->sites.size(); i++)
	{
		Site* curr = voro->sites[i];
		bool edge = (std::find_if(curr->edges.begin(), curr->edges.end(), [xMax, zMax](GraphEdge* g)
		{
			return g->x1 == 0.0f || g->x2 == 0.0f || g->y1 == 0.0f || g->y2 == 0.0f || g->x1 == xMax || g->x2 == xMax || g->y1 == zMax || g->y2 == zMax;
		}) != curr->edges.end());
		curr->water = edge;
		curr->ocean = edge;
	}

	//step 4: apply noise to determine which other nodes are water
	glm::vec2 centre = glm::vec2(xMax / 2.0f, zMax / 2.0f);
	float lengthToEdge = xMax / 2.0f;
	for (unsigned int i = 0; i < voro->sites.size(); i++)
	{
		Site* curr = voro->sites[i];
		float pFlag = lookUpHeightValue(curr->coord.x, curr->coord.y, COAST_NOISE_FREQUENCY);
		float distanceToCenter = glm::distance(curr->coord, centre) / lengthToEdge;
		if (pFlag < WATER_THRESHOLD_BASE + (ISLAND_FALLOFF * distanceToCenter * distanceToCenter))
		{
			curr->water = true;
		}
	}

	//step 5: all water nodes accessible from the borders become ocean (the rest are lakes)

	//step 6: determine elevation at corners
	for (unsigned int i = 0; i < voro->corners.size(); i++)
	{
		Corner* c = voro->corners[i];
		bool anyWater = false;
		bool allWater = true;
		//step A: Look at the adjacent sites  to figure out if this corner is water, coastal, or inland
		for (unsigned int j = 0; j < c->touches.size(); j++)
		{
			if (c->touches[j]->water)
			{
				anyWater = true;
			}
			else
			{
				allWater = false;
			}
		}
		if (allWater)
		{
			//underwater corner. Determine depth
			float distance = distanceToMatchingSite(c, [](Site* s)
			{
				return !s->water;
			});
			float dFactor = distance / lengthToEdge;
			c->elevation = 0.0f - (500.0f * dFactor);
		}
		else if (anyWater)
		{
			//coastal corner. 0 height
			c->elevation = 0.0f;
		}
		else
		{
			//inland corner. determine elevation
			float distance = distanceToMatchingSite(c, [](Site* s)
			{
				return s->water;
			});
			float dFactor = distance / lengthToEdge;
			c->elevation = 500.0f * dFactor;
		}
	}

	//STEP 2A: TEMPORARY Assign a height to each site based on the average of it's corners
	//for (unsigned int i = 0; i < voro->sites.size(); i++)
	//{
	//	//float f = lookUpHeightValue(voro->sites[i]->coord.x, voro->sites[i]->coord.y, 0.01f) * 255.0f;
	//	float f = 0.0f;
	//	for (unsigned int j = 0; j < voro->sites[i]->corners.size(); j++)
	//	{
	//		f += voro->sites[i]->corners[j]->elevation;
	//	}
	//	f /= (float)voro->sites[i]->corners.size();
	//	voro->sites[i]->elevation = f;
	//}

	//step 7: determine the downslope at corners

	//step 8: place rivers

	//step 9: determine climate
	for (unsigned int i = 0; i < voro->sites.size(); i++)
	{
		Site* curr = voro->sites[i];
		//step 9a: determine moisture and temperature per node
		//for testing, we're just going to use a simple linear scaling moisture/temperature
		//values range from 1 to 100 (as a % of maximum - not degrees)
		float xFrac = curr->coord.x / voro->borderMaxX;
		float yFrac = curr->coord.y / voro->borderMaxY;
		curr->moisture = 100.0f * xFrac;	//by adjusting this equation we can change the moisture direction
		curr->temperature = 100.0f * yFrac;
		//step 9b: determine node biome
		curr->biome = selectBiome(curr->moisture, curr->temperature);
	}	
}

//Determines to which voronoi cell the Geo/Climate point at at given index belongs
Site* WorldInfo::findVoronoiCellAtIndex(int x, int z)
{
	glm::vec2 pos = glm::vec2((float)x, (float)z);
	//do a binary search to find the voronoi point that is closest ON THE Y (Z in world space) AXIS to our current point
	int bottom = 0;
	int mid = voro->sites.size() / 2;
	int top = voro->sites.size() - 1;
	while (top - bottom > 1)
	{
		if (pos.y < voro->sites[mid]->coord.y)
		{
			top = mid;
		}
		else if (pos.y > voro->sites[mid]->coord.y)
		{
			bottom = mid;
		}
		else
		{
			//equal to the y value at mid, therefore we have found it
			break;
		}
		mid = bottom + ((top - bottom) / 2);
	}
	//at this point, one of bottom, mid, or top will refer to the closest point
	float midLength = fabs(pos.y - voro->sites[mid]->coord.y);
	if (pos.y - voro->sites[bottom]->coord.y < midLength)
	{
		mid = bottom;
	}
	else if (voro->sites[top]->coord.y - pos.y < midLength)
	{
		mid = top;
	}

	//we've the one that's closest on the y axis, now find the closest overall (note that left and right here refer to positions in the vector)
	Site* closest = voro->sites[mid];
	float initialSearchRange = glm::length(pos - closest->coord);	//search range is the maximum y distance we can look in either direction
	float searchRange = initialSearchRange;

	//start by searching to the right
	float rightSearchDistanceY = 0.0f;						//how far we've looked to the right
	int rightIndex = mid + 1;								//the index of the closest point to the right of mid
	float rangeToRight = initialSearchRange;				//the distance to the above point	
	Site* closestRight = closest;							//the actual point to the right that's closest
	while (rightSearchDistanceY < searchRange && rightIndex < voro->sites.size())
	{
		rightSearchDistanceY = voro->sites[rightIndex]->coord.y - pos.y;
		rangeToRight = glm::length(pos - voro->sites[rightIndex]->coord);
		if (rangeToRight < searchRange)
		{
			searchRange = rangeToRight;
			closestRight = voro->sites[rightIndex];
		}
		rightIndex++;
	}
	float actualRangeToRight = searchRange;		//the distance to the point that is the closest on the right side

	//now search to the left
	float leftSearchDistanceY = 0.0f;						//how far we've looked to the left
	int leftIndex = mid - 1;								//the index of the closest point to the left of mid
	float rangeToLeft = initialSearchRange;					//the distance to point we most recently looked at
	Site* closestLeft = closest;							//the actual point to the right that's closest
	while (leftSearchDistanceY < searchRange && leftIndex >= 0)
	{	
		leftSearchDistanceY = pos.y - voro->sites[leftIndex]->coord.y;
		rangeToLeft = glm::length(pos - voro->sites[leftIndex]->coord);
		if (rangeToLeft < searchRange)
		{
			searchRange = rangeToLeft;
			closestLeft = voro->sites[leftIndex];
		}
		leftIndex--;
	}
	float actualRangeToLeft = searchRange;	//the distance to the point that is the closest on the left side

	if (actualRangeToLeft < actualRangeToRight)
	{
		return closestLeft;
	}
	else
	{
		return closestRight;
	}
}

//searches through the ordered list of sites to find a the site closest to the supplied corner which matches the given predicate
float WorldInfo::distanceToMatchingSite(Corner* corner, std::function<bool(Site*)> predicate)
{
	int bottom = 0;
	int mid = voro->sites.size() / 2;
	int top = voro->sites.size() - 1;
	while (top - bottom > 1)
	{
		if (corner->pos.y < voro->sites[mid]->coord.y)
		{
			top = mid;
		}
		else if (corner->pos.y > voro->sites[mid]->coord.y)
		{
			bottom = mid;
		}
		else
		{
			//equal to the y value at mid, therefore we have found it
			break;
		}
		mid = bottom + ((top - bottom) / 2);
	}
	float midLength = fabs(corner->pos.y - voro->sites[mid]->coord.y);
	if (corner->pos.y - voro->sites[bottom]->coord.y < midLength)
	{
		mid = bottom;
	}
	else if (voro->sites[top]->coord.y - corner->pos.y < midLength)
	{
		mid = top;
	}
	Site* closest = voro->sites[mid];
	//we have the site that is closest on the Y axis, now find the closest site that has water
	float initialSearchRange = 9999999.0f;	//search range is the maximum y distance we can look in either direction. Because we don't know how far to water, we start this massive
	float searchRange = initialSearchRange;

	//start by searching to the right
	bool matchOnRight = false;
	float rightSearchDistanceY = 0.0f;						//how far we've looked to the right
	int rightIndex = mid + 1;								//the index of the closest point to the right of mid
	float rangeToRight = initialSearchRange;				//the distance to the above point	
	Site* closestRight = closest;							//the actual point to the right that's closest
	while (rightSearchDistanceY < searchRange && rightIndex < voro->sites.size())
	{
		rightSearchDistanceY = voro->sites[rightIndex]->coord.y - corner->pos.y;
		if (predicate(voro->sites[rightIndex]))
		{
			rangeToRight = glm::length(corner->pos - voro->sites[rightIndex]->coord);
			if (rangeToRight < searchRange)
			{
				searchRange = rangeToRight;
				closestRight = voro->sites[rightIndex];
				matchOnRight = true;
			}
		}
		rightIndex++;
	}
	float actualRangeToRight = searchRange;		//the distance to the point that is the closest on the right side

	//now search to the left
	bool matchOnLeft = false;
	float leftSearchDistanceY = 0.0f;						//how far we've looked to the left
	int leftIndex = mid - 1;								//the index of the closest point to the left of mid
	float rangeToLeft = initialSearchRange;					//the distance to point we most recently looked at
	Site* closestLeft = closest;							//the actual point to the right that's closest
	while (leftSearchDistanceY < searchRange && leftIndex >= 0)
	{
		leftSearchDistanceY = corner->pos.y - voro->sites[leftIndex]->coord.y;
		if (predicate(voro->sites[leftIndex]))
		{
			rangeToLeft = glm::length(corner->pos - voro->sites[leftIndex]->coord);
			if (rangeToLeft < searchRange)
			{
				searchRange = rangeToLeft;
				closestLeft = voro->sites[leftIndex];
				matchOnLeft = true;
			}
		}
		leftIndex--;
	}
	float actualRangeToLeft = searchRange;	//the distance to the point that is the closest on the left side

	if (actualRangeToLeft <= actualRangeToRight && matchOnLeft)
	{
		return actualRangeToLeft;
	}
	else if (actualRangeToRight <= actualRangeToLeft && matchOnRight)
	{
		return actualRangeToRight;
	}
	else
	{
		DEBUG_PRINT("Could not find matching site. Undefined behavior\n");
		return 0.0f;
	}
}

float WorldInfo::heightOfPointInSite(int siteX, int siteZ, Site* site)
{
	//iterate over the site until we have found the triangle that contains the point
	glm::vec2 pos = glm::vec2((float)siteX, (float)siteZ);
	Corner* p0 = site->corners[0];
	Corner* p1 = site->corners[1];
	Corner* p2 = site->corners[2];
	bool pit = false;
	for (unsigned int i = 1; i < site->corners.size() - 1; i++)
	{
		p1 = site->corners[i];
		for (unsigned int j = i + 1; j < site->corners.size(); j++)
		{
			p2 = site->corners[j];
			if (pointInTriangle(pos, p0->pos, p1->pos, p2->pos))
			{
				pit = true;
				break;
			}
		}
		if (pit)
		{
			break;
		}
	}

	//at this point, we have a triangle that contains the point. time to calculate the height of the point in the triangle
	glm::vec3 P = glm::vec3(p0->pos.x, p0->pos.y, p0->elevation);
	glm::vec3 Q = glm::vec3(p1->pos.x, p1->pos.y, p1->elevation);
	glm::vec3 R = glm::vec3(p2->pos.x, p2->pos.y, p2->elevation);
	glm::vec3 N = glm::normalize(glm::cross(R - P, Q - P));	//plane normal vector
	float height = (((N.x * (pos.x - P.x)) + (N.y * (pos.y - P.y))) / (-N.z)) + P.z;
	return height;
}

void WorldInfo::initialiseBiomes()
{
	//generate the biomes
	tundra = new Biome(TUNDRA, glm::vec3(240, 240, 240), "Tundra", 0.5f, 0.5f, 0.5f, 0.8f);
	desert = new Biome(DESERT, glm::vec3(212, 210, 144), "Desert", 200.0f, 200.0f, 0.6f, 0.7f);
	grassland = new Biome(GRASSLAND, glm::vec3(82, 230, 262), "Grassland", 1.0f, 200.0f, 0.7f, 0.8f);
	taiga = new Biome(TAIGA, glm::vec3(62, 230, 202), "Taiga", 0.5f, 0.5f, 0.5f, 0.8f);
	swamp = new Biome(SWAMP, glm::vec3(64, 87, 17), "Swamp", 0.5f, 200.0f, 0.6f, 0.8f);
	forest = new Biome(FOREST, glm::vec3(20, 117, 22), "Forest", 1.0f, 200.0f, 0.5f, 0.8f);
	jungle = new Biome(JUNGLE, glm::vec3(54, 214, 0), "Jungle", 1.5f, 200.0f, 0.45f, 0.8f);
	savannah = new Biome(SAVANNAH, glm::vec3(188, 214, 15), "Savannah", 1.0f, 200.0f, 0.4f, 0.75f);
	badlands = new Biome(BADLAND, glm::vec3(150, 132, 48), "Badland", 2.0f, 200.0f, 0.0f, 0.5f);
	marsh = new Biome(MARSH, glm::vec3(134, 168, 93), "Marsh", 0.5f, 200.0f, 0.6f, 0.8f);
	alpine = new Biome(ALPINE, glm::vec3(164, 231, 235), "Alpine", 0.5f, 0.5f, 0.5f, 0.8f);
	ashland = new Biome(ASHLAND, glm::vec3(50, 50, 50), "Ashland", 2.0f, 200.0f, 0.0f, 0.5f);

	//generate a vector containing the biomes (serves as a mapping from id->biome)
	biomes.reserve(12);
	//it is CRITICAL that biomes be added to this vector in the same order they appear in the biomes enum
	biomes.push_back(tundra);
	biomes.push_back(desert);
	biomes.push_back(grassland);
	biomes.push_back(taiga);
	biomes.push_back(swamp);
	biomes.push_back(forest);
	biomes.push_back(jungle);
	biomes.push_back(savannah);
	biomes.push_back(badlands);
	biomes.push_back(marsh);
	biomes.push_back(alpine);
	biomes.push_back(ashland);

	//generate a map from biome name -> biome
	biomesByName.emplace("TUNDRA", tundra);
	biomesByName.emplace("DESERT", desert);
	biomesByName.emplace("GRASSLAND", grassland);
	biomesByName.emplace("TAIGA", taiga);
	biomesByName.emplace("SWAMP", swamp);
	biomesByName.emplace("FOREST", forest);
	biomesByName.emplace("JUNGLE", jungle);
	biomesByName.emplace("SAVANNAH", savannah);
	biomesByName.emplace("BADLANDS", badlands);
	biomesByName.emplace("MARSH", marsh);
	biomesByName.emplace("ALPINE", alpine);
	biomesByName.emplace("ASHLAND", ashland);
}

void WorldInfo::initialiseTrees()
{
	//load the species of tree
	std::string line;
	std::ifstream fs("./Data/DataFiles/TreeTypes.txt");
	std::map<std::string, Biome*>::iterator it;
	while (std::getline(fs, line))
	{
		if (!line.empty() && line.at(0) != '#')
		{
			//the line represents a species of tree
			std::string file = "./Data/DataFiles/" + line + ".txt";
			TreeSpecies* species = new TreeSpecies(file.c_str(), resourceManager);
			treeSpecies.push_back(species);
			//add this tree to biomes
			for (auto& tag : splitString(species->biomesTag, ','))
			{
				if ((it = biomesByName.find(tag)) != biomesByName.end())
				{
					it->second->addTreeType(species);
				}
			}

		}
	}

	//generate a mapping of species to instances of trees of that species
	for (auto& species : treeSpecies)
	{
		std::vector<Tree*> treeVariants;
		treeVariants.reserve(TREE_VARIANTS);
		for (int i = 0; i < TREE_VARIANTS; i++)
		{
			treeVariants.push_back(new Tree(species));
		}
		trees.emplace(species, treeVariants);
	}
}

void WorldInfo::initialiseRocks()
{
	std::string line;
	std::ifstream fs("./Data/DataFiles/RockTypes.txt");
	while (std::getline(fs, line))
	{
		if (!line.empty() && line.at(0) != '#')
		{
			//the line represents a species of tree
			std::string file = "./Data/DataFiles/" + line + ".txt";
			rockTypes.push_back(new RockType(file.c_str(), resourceManager));
		}
	}
	rocks.reserve(ROCK_VARIANTS * rockTypes.size());
	for (auto& rockType : rockTypes)
	{
		for (int i = 0; i < ROCK_VARIANTS; i++)
		{
			rocks.push_back(new Rock(rockType));
		}
	}
}

Biome* WorldInfo::selectBiome(float moisture, float temperature)
{
	if (temperature > 75.0f)
	{
		if (moisture > 75.0f)
		{
			return jungle;
		}
		if (moisture > 50.0f)
		{
			return jungle;
		}
		else if (moisture > 25.0f)
		{
			return desert;
		}
		else
		{
			return desert;
		}
	}
	else if (temperature > 50.0f)
	{
		if (moisture > 75.0f)
		{
			return marsh;
		}
		else if (moisture > 50.0f)
		{
			return grassland;
		}
		else if (moisture > 25.0f)
		{
			return savannah;
		}
		else
		{
			return badlands;
		}
	}
	else if (temperature > 25.0f)
	{
		if (moisture > 75.0f)
		{
			return swamp;
		}
		else if (moisture > 50.0f)
		{
			return forest;
		}
		else if (moisture > 25.0f)
		{
			return forest;
		}
		else
		{
			return grassland;
		}
	}
	else
	{
		if (moisture > 75.0f)
		{
			return taiga;
		}
		else if (moisture > 50.0f)
		{
			return taiga;
		}
		else if (moisture > 25.0f)
		{
			return tundra;
		}
		else
		{
			return tundra;
		}
	}
}

std::vector<std::string> WorldInfo::splitString(const std::string &s, char delimiter)
{
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delimiter)) 
	{
		elems.push_back(item);
	}
	return elems;
}

float WorldInfo::noiseMapLookup(int x, int z)
{
	int x1 = x % NOISEMAP_X;
	int z1 = z % NOISEMAP_Z;
	return noiseMap[x1 + (NOISEMAP_X * z1)];
}