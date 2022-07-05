#include "AddRocksTask.h"

AddRocksTask::AddRocksTask(int nodeIndex, Node* target, Scene* scene, WorldInfo* info)
{
	this->nodeIndex = nodeIndex;
	this->node = target;
	this->scene = scene;
	this->worldInfo = info;
}

void AddRocksTask::execute()
{
	std::vector<GameObjectReference*> rocksToAdd;
	int rockTypeCount = worldInfo->rocks.size();
	//we assume that the node is in the built state, so all data about the node can be used
	int nodeXIndex = nodeIndex % NODES_PER_WORLD_SIDE;
	int nodeZIndex = nodeIndex / NODES_PER_WORLD_SIDE;

	//start off simple: add some rocks to the node
	float nodeX = (float)(HEIGHTMAP_TEXTURE_WIDTH * nodeXIndex);
	float nodeZ = (float)(HEIGHTMAP_TEXTURE_WIDTH * nodeZIndex);

	for (int x = 5; x < HEIGHTMAP_TEXTURE_WIDTH; x += 16)
	{
		for (int z = 3; z < HEIGHTMAP_TEXTURE_WIDTH; z += 16)
		{
			//determine biome at this point
			int geoX = (int)((nodeX + x) / FWORLD_POINT_SEPARATION);
			int geoZ = (int)((nodeZ + z) / FWORLD_POINT_SEPARATION);
			GeoPoint geo = worldInfo->geoAtIndex(geoX, geoZ);
			Biome* biome = geo.voronoiCell->biome;

			float r = worldInfo->noiseMapLookup(x, z);
			float offX = 4.0f - (8.0f * r);
			float offZ = 4.0f - (8.0f * r);
			float posX = (float)x + offX;
			float posZ = (float)z + offZ;
			float posY = node->heightMapData[(int)posZ + HEIGHTMAP_TEXTURE_WIDTH * (int)posX];
			
			glm::vec4 splats = node->splatMapData[(int)posZ + HEIGHTMAP_TEXTURE_WIDTH * (int)posX];
			//rocks can appear on rock, sand, and snow
			if (biome->rockAtPoint(splats, posY, r))
			{
				float scale = 0.5f + fmodf(posY, 1.0f);
				//this may be a problem - posX is always 5 + 16n
				GameObjectReference* rockRef = new GameObjectReference(worldInfo->rocks[(int)posX % ROCK_VARIANTS], nodeIndex);
				rockRef->transform->setPosition(glm::vec3(posX + nodeX, posY - 0.5f, posZ + nodeZ));
				rockRef->transform->setScale(glm::vec3(scale, scale, scale));
				rockRef->transform->setRotation(glm::vec4(0.0f, 1.0f, 0.0f, 360.0f * r));
				//scene->addObjectReference(rockRef);
				rocksToAdd.push_back(rockRef);
			}
		}
	}

	scene->addObjectReferenceBatch(rocksToAdd);
}