#ifndef ENGINE01_CUBE_H
#define ENGINE01_CUBE_H

#include "GL\glew.h"
#include "glm\glm.hpp"
#include <vector>

#include "Camera.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "Transform.h"
#include "Vertex.h"

//Represents a simple cubic mesh object. Useful for testing.

class Cube : public GameObject
{
private:
	Mesh* mesh;
	void buildCubeMesh();
public:
	Cube(Material* material);
	~Cube();
	void draw(int renderPass, Transform* transform);
};

#endif