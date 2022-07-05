#ifndef ENGINE01_WORLD_H
#define ENGINE01_WORLD_H

//Represents storage of a world environment
//Stores data about nodes, and owns a WorldInfo object which contains all the geography and climate info used to build these nodes

#include <chrono>
#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glutils.h"
#include <iostream>
#include <thread>
#include <vector>

#include "AddTreesTask.h"
#include "AddRocksTask.h"
#include "BuildNodeTask.h"
#include "Camera.h"
#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include "Math.h"
#include "Node.h"
#include "Noise.h"
#include "Scene.h"
#include "Settings.h"
#include "Shader.h"
#include "Skybox.h"
#include "StopWatch.h"
#include "TaskManager.h"
#include "Vertex.h"
#include "Voronoi.h"
#include "WorldInfo.h"
#include "WorldMap.h"

static char* arrowPath = "Data/arrow.tga";
static char* rockPath = "Data/2Rock128.tga";
static char* grassPath = "Data/4Grass128.tga";
static char* dirtPath = "Data/Dirt128.tga";
static char* snowPath = "Data/Snow128.tga";

struct NodeGrid	//North is greater z-value, west is greater x-value
{
	int Center;
	int North;
	int South;
	int East;
	int West;
	int NE;
	int NW;
	int SE;
	int SW;
	inline std::vector<int> iterate()
	{
		return std::vector<int>({ SE, South, SW, East, Center, West, NE, North, NW });
	}
};

class World
{
private:
	WorldInfo* worldInfo;
	GraphicsResourceManager* resourceManager;
	TaskManager* taskManager;
	Scene* scene;
	UIElement* mapMarker;
	void loadEnvironmentTexture(const char * path, GLuint * textureHandle);
	void unloadNodes(std::vector<int> previousNodes);
	void setNodeGrid(int cameraXIndex, int cameraZIndex, int cameraIndex);
	void nodeUpdate(Camera* camera);
	void updateNodeGrid(Camera* camera);
	void mapUpdate(Camera* camera);
public:
	World(TaskManager* manager, Scene* scene, GraphicsResourceManager* resourceManager);
	~World();
	void update(Camera* camera);
	NodeGrid grid;
	Node* nodes;
	float getTerrainHeightAtPoint(float x, float z);
	WorldMap* generateWorldMap();
};

#endif