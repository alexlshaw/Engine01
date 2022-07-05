#ifndef ENGINE01_ROCK_H
#define ENGINE01_ROCK_H

#include "glm\glm.hpp"

#include "Collision.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "Noise.h"
#include "RockType.h"
#include "Transform.h"
#include "Vertex.h"

class Rock : public GameObject
{
private:
	Mesh* mesh;
	void buildMesh();
	void buildMeshScatterMethod(RockType* rockType);
public:
	Rock(RockType* rockType);
	~Rock();
	void draw(int renderPass, Transform* transform);
};

#endif