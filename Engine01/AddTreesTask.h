#ifndef ENGINE01_ADDTREESTASK_H
#define ENGINE01_ADDTREESTASK_H

#include "glm\glm.hpp"
#include <vector>

#include "Node.h"
#include "Scene.h"
#include "Task.h"
#include "WorldInfo.h"

class AddTreesTask : public Task
{
private:
	Node* node;
	Scene* scene;
	int nodeIndex;
	WorldInfo* worldInfo;
public:
	AddTreesTask(int nodeIndex, Node* target, Scene* scene, WorldInfo* info);
	virtual void execute() override;
};

#endif