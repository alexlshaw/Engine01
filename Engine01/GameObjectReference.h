#ifndef ENGINE01_GAMEOBJECTREFRENCE_H
#define ENGINE01_GAMEOBJECTREFRENCE_H

//this class stores the transformation info of a specific instance of a gameobject
//Thus, when drawing the same object in multiple different positions, we only have to store most of the info about the object once

#include "glm\glm.hpp"
#include "glm\gtx\transform.hpp"

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"

class GameObjectReference
{
private:
public:
	bool cullingFlag = false;
	int cullingPlaneCache = -1;
	int tag;
	GameObjectReference(GameObject* target);
	GameObjectReference(GameObject* target, int tag);
	~GameObjectReference();
	GameObject* base;
	Transform* transform;
	void draw(int renderPass);
};

#endif