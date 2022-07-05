#ifndef ENGINE01_ADDROCKSTASK_H
#define ENGINE01_ADDROCKSTASK_H

#include "glm\glm.hpp"
#include <vector>

#include "Node.h"
#include "Scene.h"
#include "Task.h"
#include "WorldInfo.h"

class AddRocksTask : public Task
{
private:
	Node* node;
	Scene* scene;
	int nodeIndex;
	WorldInfo* worldInfo;
public:
	AddRocksTask(int nodeIndex, Node* target, Scene* scene, WorldInfo* info);
	virtual void execute() override;
};

#endif