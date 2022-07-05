#include "GameObject.h"

int GameObject::getRenderPasses()
{
	return materials.size();
}

void GameObject::draw(int renderPass, Transform* transform)
{
	DEBUG_PRINT("Error: Attempting to use GameObject default draw.\n");
}

void GameObject::activateMaterial(int renderPass, Camera* camera)
{
	materials[renderPass]->use(camera);
}