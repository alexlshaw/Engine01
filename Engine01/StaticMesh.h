#pragma once

#include "GameObject.h"
#include "Mesh.h"

//A gameobject that represents a simple, unchanging mesh displayed within the scene
//Not to be confused with the Mesh class, which represents the actual vertex graph
class StaticMesh : public GameObject
{
private:
	Mesh mesh;
public:
	StaticMesh(Mesh* mesh, Material* material);
	void draw(int renderPass, Transform* transform);
};