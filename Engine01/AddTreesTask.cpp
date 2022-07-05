#include "AddTreesTask.h"

AddTreesTask::AddTreesTask(int nodeIndex, Node* target, Scene* scene, WorldInfo* info)
{
	this->nodeIndex = nodeIndex;
	this->node = target;
	this->scene = scene;
	this->worldInfo = info;
}

void AddTreesTask::execute()
{
	std::vector<GameObjectReference*> treesToAdd;
	//load meshes like trees etc into the scene for the given node
	//we assume that the node is in the built state, so all data about the node can be used
	int nodeXIndex = nodeIndex % NODES_PER_WORLD_SIDE;
	int nodeZIndex = nodeIndex / NODES_PER_WORLD_SIDE;

	//start off simple: add some trees to the node
	float nodeX = (float)(HEIGHTMAP_TEXTURE_WIDTH * nodeXIndex);
	float nodeZ = (float)(HEIGHTMAP_TEXTURE_WIDTH * nodeZIndex);

	for (int x = 7; x < HEIGHTMAP_TEXTURE_WIDTH; x += 16)
	{
		for (int z = 7; z < HEIGHTMAP_TEXTURE_WIDTH; z += 16)
		{
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
			if (biome->treeAtPoint(splats, posY, r))
			{
				//we are placing a tree, find out which one
				TreeSpecies* species = biome->pickTreeType(r);
				Tree* tree = worldInfo->trees[species][(int)(r * 1000.0f) % TREE_VARIANTS];
				GameObjectReference* treeRef = new GameObjectReference(tree, nodeIndex);
				float scale = 0.5f + fmodf(posY, 1.0f);
				treeRef->transform->setPosition(glm::vec3(posX + nodeX, posY - 0.5f, posZ + nodeZ));
				treeRef->transform->setScale(glm::vec3(scale, scale, scale));
				treeRef->transform->setRotation(glm::vec4(0.0f, 1.0f, 0.0f, 360.0f * r));
				treeRef->transform->getMatrix();	//force an update of the matrix so we don't have to do it when drawing
				treesToAdd.push_back(treeRef);
			}
		}
	}

	scene->addObjectReferenceBatch(treesToAdd);
}