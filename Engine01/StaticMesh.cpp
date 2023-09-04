#include "StaticMesh.h"

StaticMesh::StaticMesh(Mesh* mesh, Material* material)
{
	//Probably need to place a default material here
	this->mesh = *mesh;
	materials.push_back(material);
}

void StaticMesh::draw(int renderPass, Transform* transform)
{
	materials[renderPass]->setTransform(transform);
	if (renderPass == 0)
	{
		mesh.draw();
	}
}