#include "Cube.h"

Cube::Cube(Material* material)
{
	mesh = nullptr;
	materials.push_back(material);
	//build the mesh
	buildCubeMesh();
}

Cube::~Cube()
{
	//destroy the mesh
	if (mesh != nullptr)
	{
		delete mesh;
		mesh = nullptr;
	}
}

void Cube::buildCubeMesh()
{
	//create the vertex list
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;
	MeshTools::addCube(&vertices, &indices, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
	//push the mesh
	mesh = new Mesh(vertices, indices);
}

void Cube::draw(int renderPass, Transform* transform)
{
	materials[renderPass]->setTransform(transform);
	mesh->draw();
}