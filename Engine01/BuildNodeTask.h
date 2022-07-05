#ifndef ENGINE01_BUILDNODETASK_H
#define ENGINE01_BUILDNODETASK_H

#include "glm\glm.hpp"

#include "Biome.h"
#include "DebuggingTools.h"
#include "Node.h"
#include "Settings.h"
#include "Task.h"
#include "WorldInfo.h"

class BuildNodeTask : public Task
{
private:
	int index;
	int startingX, startingZ, xMax, zMax;
	Node* node;
	WorldInfo* worldInfo;
	void buildHeightMap();
	void buildSplatMap();
	float determineSlope(int x, int z, float height);
	float getHeight(int x, int z);	//IMPORTANT NOTE: Only call this method after building the heightmap data
public:
	BuildNodeTask(int nodeIndex, Node* nodeToBuild, WorldInfo* info);
	virtual void execute() override;
};

#endif