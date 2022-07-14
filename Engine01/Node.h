#ifndef ENGINE01_NODE_H
#define ENGINE01_NODE_H

//This is a holder class for information about a block of terrain data

#include "glad/glad.h"
#include "glm\glm.hpp"
#include <vector>

#include "Settings.h"

enum NodeState { UNBUILT, BUILDING, BUILT, HM_PUSHED, READY };
//UNBUILT: no work has been done on the node
//BUILDING: the node has been queued for construction by the worker thread (construction may or may not have actually begun)
//BUILT: the data for the heightmap has been generated, the main thread needs to push it to openGL
//HM_PUSHED: the heightmap has been pushed, the main thread still needs to push the splatmap to openGL
//READY: the node is complete, and can be drawn

class Node
{
private:	
public:
	Node();
	~Node();
	NodeState state;
	float buildTime;
	GLuint heightMapTexture, splatMapTexture;
	std::vector<float> heightMapData;
	std::vector<glm::vec4> splatMapData;
	void clear();
	void pushHeightmap();
	void pushSplatMap();
};

#endif