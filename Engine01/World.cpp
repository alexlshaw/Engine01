#include "World.h"

World::World(TaskManager* manager, Scene* scene, GraphicsResourceManager* resourceManager)
{
	//link the task manager
	taskManager = manager;
	this->resourceManager = resourceManager;
	this->scene = scene;
	//generate terrain stuff
	nodes = new Node[TOTAL_NODES];
	setNodeGrid(0, 0, 0);
	worldInfo = new WorldInfo(resourceManager);
}

World::~World()
{
	DEBUG_PRINT("Destroying world\n");
	delete[] nodes;
	if (worldInfo != nullptr)
	{
		delete worldInfo;
		worldInfo = nullptr;
	}
}

void World::update(Camera* camera)
{
	nodeUpdate(camera);
	mapUpdate(camera);
}

void World::nodeUpdate(Camera* camera)
{
	updateNodeGrid(camera);
	bool doneOneThingThisFrame = false;
	//guarantee that center is built
	if (nodes[grid.Center].state != READY)
	{
		if (nodes[grid.Center].state == UNBUILT)
		{
			DEBUG_PRINT("Center node unbuilt, queueing and waiting for it to complete\n");
			//buildHeightMapTexture(&nodes[grid.Center], grid.Center);	//this also pushes the heightmap
			nodes[grid.Center].state = BUILDING;
			taskManager->queueTask(new BuildNodeTask(grid.Center, &nodes[grid.Center], worldInfo));
			while (nodes[grid.Center].state != BUILT)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			nodes[grid.Center].pushHeightmap();
			nodes[grid.Center].state = HM_PUSHED;
			nodes[grid.Center].pushSplatMap();
		}
		else if (nodes[grid.Center].state == BUILDING)
		{
			//this is an awkward situation where the node has already been queued and may be half built already, so we just have to wait for the worker thread to finish it
			//If we encounter this situation we can expect a bit of lag, but hopefully this won't ever actually come up
			DEBUG_PRINT("Center node already in building state, have to wait\n");
			while (nodes[grid.Center].state != BUILT)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			nodes[grid.Center].pushHeightmap();
			nodes[grid.Center].state = HM_PUSHED;
			nodes[grid.Center].pushSplatMap();
		}
		else if (nodes[grid.Center].state == BUILT)
		{
			nodes[grid.Center].pushHeightmap();
			nodes[grid.Center].state = HM_PUSHED;
			nodes[grid.Center].pushSplatMap();
		}
		nodes[grid.Center].state = READY;
		taskManager->queueTask(new AddTreesTask(grid.Center, &nodes[grid.Center], scene, worldInfo));
		taskManager->queueTask(new AddRocksTask(grid.Center, &nodes[grid.Center], scene, worldInfo));
		doneOneThingThisFrame = true;
	}


	//iterate over all the nodes in the grid, if they are unbuilt, queue a task to build them

	for (int& nodeIndex : grid.iterate())
	{
		if (doneOneThingThisFrame)
		{
			break;
		}
		if (nodes[nodeIndex].state == UNBUILT)
		{
			DEBUG_PRINT("Queuing node\n");
			nodes[nodeIndex].state = BUILDING;
			taskManager->queueTask(new BuildNodeTask(nodeIndex, &nodes[nodeIndex], worldInfo));
			//doneOneThingThisFrame = true;	//this thing is negligible enough that we probably don't have to limit it to one per frame
		}
		else if (nodes[nodeIndex].state == BUILT)
		{
			DEBUG_PRINT("Pushing node heightmap\n");
			nodes[nodeIndex].pushHeightmap();
			nodes[nodeIndex].state = HM_PUSHED;
			doneOneThingThisFrame = true;
		}
		else if (nodes[nodeIndex].state == HM_PUSHED)
		{
			DEBUG_PRINT("Pushing node splatmap\n");
			nodes[nodeIndex].pushSplatMap();
			nodes[nodeIndex].state = READY;
			taskManager->queueTask(new AddTreesTask(nodeIndex, &nodes[nodeIndex], scene, worldInfo));
			taskManager->queueTask(new AddRocksTask(nodeIndex, &nodes[nodeIndex], scene, worldInfo));
			doneOneThingThisFrame = true;
		}
	}
}

void World::updateNodeGrid(Camera* camera)
{
	//in this method, step 1 is to identify the Node the camera is inside
	int cameraXIndex = (int)camera->position.x / HEIGHTMAP_TEXTURE_WIDTH;
	int cameraZIndex = (int)camera->position.z / HEIGHTMAP_TEXTURE_WIDTH;
	int cameraIndex = cameraXIndex + (NODES_PER_WORLD_SIDE * cameraZIndex);
	//Only update the grid if the camera has moved into a different node
	if (grid.Center != cameraIndex)
	{
		std::vector<int> previousNodes = grid.iterate();
		setNodeGrid(cameraXIndex, cameraZIndex, cameraIndex);
		//if the centre changed, some of the nodes we have loaded will have changed
		unloadNodes(previousNodes);
	}
}

void World::mapUpdate(Camera* camera)
{
	float mapX = (float)(WORLD_POINTS_PER_SIDE) - (camera->position.x / FWORLD_POINT_SEPARATION);
	float mapZ = camera->position.z / FWORLD_POINT_SEPARATION;
	glm::vec2 currSize = mapMarker->getSize();
	mapMarker->resize(mapX, mapZ, currSize);
}

void World::setNodeGrid(int cameraXIndex, int cameraZIndex, int cameraIndex)
{
	
	grid.Center = cameraIndex;
	grid.North = cameraIndex + NODES_PER_WORLD_SIDE;
	grid.South = cameraIndex - NODES_PER_WORLD_SIDE;
	grid.East = cameraIndex - 1;
	grid.West = cameraIndex + 1;
	grid.NE = cameraIndex + NODES_PER_WORLD_SIDE - 1;
	grid.NW = cameraIndex + NODES_PER_WORLD_SIDE + 1;
	grid.SE = cameraIndex - NODES_PER_WORLD_SIDE - 1;
	grid.SW = cameraIndex - NODES_PER_WORLD_SIDE + 1;
	//check for cases where the camera is in an edge node
	if (cameraXIndex == 0)
	{
		//we have no eastern nodes
		grid.East = grid.Center;
		grid.NE = grid.Center;
		grid.SE = grid.Center;
	}
	else if (cameraXIndex == NODES_PER_WORLD_SIDE - 1)
	{
		//we have no western nodes
		grid.West = grid.Center;
		grid.NW = grid.Center;
		grid.SW = grid.Center;
	}
	if (cameraZIndex == 0)
	{
		//we have no southern nodes
		grid.South = grid.Center;
		grid.SE = grid.Center;
		grid.SW = grid.Center;
	}
	else if (cameraZIndex == NODES_PER_WORLD_SIDE - 1)
	{
		//we have no northern nodes
		grid.North = grid.Center;
		grid.NE = grid.Center;
		grid.NW = grid.Center;
	}
}

void World::unloadNodes(std::vector<int> previousNodes)
{
	std::vector<int> currentNodes = grid.iterate();
	for (auto& i : previousNodes)
	{
		//if i is not in the current node set, node i must be unloaded
		if (!std::any_of(currentNodes.begin(), currentNodes.end(), [i](int j){return j == i; }))
		{
			nodes[i].clear();
			scene->deleteReferencesByTag(i);
		}
	}
}

float World::getTerrainHeightAtPoint(float x, float z)
{
	return worldInfo->getTerrainHeightAtPoint(x, z);
}

WorldMap* World::generateWorldMap()
{
	//read worldInfo to generate the texture data
	//note that index 0 in the texture data is the top left corner
	//but 0,0 in world space is the bottom left corner
	std::vector<GLubyte> defaultMapPixels;
	defaultMapPixels.reserve(WORLD_MAP_TEXTURE_WIDTH * WORLD_MAP_TEXTURE_WIDTH);
	std::vector<GLubyte> biomeMapPixels;
	biomeMapPixels.reserve(WORLD_MAP_TEXTURE_WIDTH * WORLD_MAP_TEXTURE_WIDTH);


	GLubyte r0, g0, b0, a0 = 255;
	GLubyte r1, g1, b1, a1 = 255;
	//we generate all map types in one run to save time on cell lookups
	for (int z = WORLD_MAP_TEXTURE_WIDTH - 1; z >= 0; z--)
	{
		for (int x = WORLD_MAP_TEXTURE_WIDTH - 1; x >= 0; x--)
		{
			GeoPoint point = worldInfo->geoAtIndex(x, z);

			if (point.voronoiCell->water)
			{
				r0 = 0;
				g0 = 0;
				b0 = 255 - (byte)(glm::min<float>(128.0f, -point.topography));
				r1 = 0;
				g1 = 0;
				b1 = 255 - (byte)(glm::min<float>(128.0f, -point.topography));
			}
			else
			{
				byte bt = (byte)point.topography;
				r0 = bt;
				g0 = bt;
				b0 = bt;
				Biome* biome = point.voronoiCell->biome;
				r1 = biome->mapColour.r;
				g1 = biome->mapColour.g;
				b1 = biome->mapColour.b;
			}

			a0 = 255;
			a1 = 255;
			defaultMapPixels.push_back(r0);
			defaultMapPixels.push_back(g0);
			defaultMapPixels.push_back(b0);
			defaultMapPixels.push_back(a0);
			biomeMapPixels.push_back(r1);
			biomeMapPixels.push_back(g1);
			biomeMapPixels.push_back(b1);
			biomeMapPixels.push_back(a1);
		}
	}

	//generate texture objects
	Texture* defaultMapTexture = new Texture();
	Texture* biomeMapTexture = new Texture();
	defaultMapTexture->loadFromPixels(defaultMapPixels, WORLD_MAP_TEXTURE_WIDTH, WORLD_MAP_TEXTURE_WIDTH);
	biomeMapTexture->loadFromPixels(biomeMapPixels, WORLD_MAP_TEXTURE_WIDTH, WORLD_MAP_TEXTURE_WIDTH);

	WorldMap* map = new WorldMap(256.0f, 0.0f, glm::vec2(512.0f, 512.0f), nullptr, defaultMapTexture, biomeMapTexture);

	Texture* markerTex = resourceManager->loadTexture("star");
	mapMarker = new UIElement(0.0f, 0.0f, glm::vec2(4.0f, 4.0f), map, markerTex);
	return map;
}