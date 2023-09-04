#ifndef ENGINE01_GAMEOBJECT_H
#define ENGINE01_GAMEOBJECT_H

#include <string>
#include <vector>

#include "Camera.h"
#include "DebuggingTools.h"
#include "Material.h"
#include "Transform.h"

//Base class representing an object that has references in the world
class GameObject
{
private:
protected:
	std::vector<Material*> materials;
public:
	virtual void draw(int renderPass, Transform* transform);
	virtual int getRenderPasses();
	virtual void activateMaterial(int renderPass, Camera* camera);
	float drawDistanceScale = DEFAULT_DRAW_SCALE;
	std::string name = "";
	bool noCull = false;	//if true, the scene never tries to cull the object if it is outside the camera's frustrum
};

#endif