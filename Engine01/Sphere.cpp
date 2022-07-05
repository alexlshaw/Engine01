#include "Sphere.h"

Sphere::Sphere(Material* material)
{
	mesh = nullptr;
	materials.push_back(material);
	//build the mesh
	buildSphereMesh();
}

Sphere::~Sphere()
{
	//destroy the mesh
	if (mesh != nullptr)
	{
		delete mesh;
		mesh = nullptr;
	}
}

void Sphere::buildSphereMesh()
{
	//create the vertex list
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;
	
	MeshTools::createSphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 12, 24, &vertices, &indices);

	
	//push the mesh
	mesh = new Mesh(vertices, indices);
}

void Sphere::draw(int renderPass, Transform* transform)
{
	materials[renderPass]->setTransform(transform);
	mesh->draw();
}