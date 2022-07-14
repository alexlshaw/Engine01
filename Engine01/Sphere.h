#ifndef ENGINE01_SPHERE_H
#define ENGINE01_SPHERE_H

#include <vector>
#include "glad/glad.h"
#include "glm\glm.hpp"
#include "Camera.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "Transform.h"
#include "Vertex.h"

//Represents a simple cubic mesh object. Useful for testing.

class Sphere : public GameObject
{
private:
	Mesh* mesh;
	void buildSphereMesh();
public:
	Sphere(Material* material);
	~Sphere();
	void draw(int renderPass, Transform* transform);
};

#endif