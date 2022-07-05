#include "GameObjectReference.h"

GameObjectReference::GameObjectReference(GameObject* target)
{
	this->transform = new Transform();
	base = target;
	tag = -1;
}

GameObjectReference::GameObjectReference(GameObject* target, int tag)
{
	this->transform = new Transform();
	this->tag = tag;
	base = target;
}

GameObjectReference::~GameObjectReference()
{
	delete transform;
}

void GameObjectReference::draw(int renderPass)
{
	base->draw(renderPass, transform);
}