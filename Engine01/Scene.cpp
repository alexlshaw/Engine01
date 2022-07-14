#include "Scene.h"

Scene::Scene(Camera* mainCamera)
{
	this->mainCamera = mainCamera;
	drawCount = 0;
	materialActivations = 0;
	totalObjects = 0;
}

Scene::~Scene()
{
	//dispose of the camera
	if (mainCamera != nullptr)
	{
		delete mainCamera;
	}
	//dispose of the scene object references and then the reference group object.
	//Does not delete the original objects, because we can't guarantee that out of a set of objects generated elsewhere, all are used in the scene
	for (auto& group : objectsInScene)
	{
		for (auto& reference : group->references)
		{
			delete reference;
		}
		delete group;
	}
}

void Scene::setCullList()
{
	mainCamera->updateFrustrum();
	sceneObjectsLock.lock();
	for (auto& group : objectsInScene)
	{
		float drawDistance = VIEW_SQUARED * group->gameObject->drawDistanceScale;
		for (unsigned int i = 0; i < group->references.size(); i++)
		{
			GameObjectReference* reference = group->references[i];
			//first check if the object is too far away
			glm::vec3 refPos = reference->transform->getPosition();
			glm::vec3 d = mainCamera->position - refPos;
			if ((d.x * d.x + d.z * d.z) > drawDistance)
			{
				reference->cullingPlaneCache = -1;
				reference->cullingFlag = true;
			}
			else
			{
				//the object was not too far away, check if it is in the frustrum
				int failurePlane = reference->cullingPlaneCache;
				int inFrustrum = mainCamera->frustrum.sphereInFrustrum(refPos, 10.0f, failurePlane);
				if (inFrustrum == OUTSIDE)
				{
					reference->cullingPlaneCache = failurePlane;
					reference->cullingFlag = true;
				}
				else
				{
					reference->cullingPlaneCache = -1;
					reference->cullingFlag = false;
				}
			}
		}
	}
	sceneObjectsLock.unlock();
}

void Scene::draw()
{
	drawCount = 0;
	materialActivations = 0;
	sceneObjectsLock.lock();
	for (auto& group : objectsInScene)
	{
		int passes = group->gameObject->getRenderPasses();
		for (int pass = 0; pass < passes; pass++)
		{
			group->gameObject->activateMaterial(pass, mainCamera);
			materialActivations++;
			for (unsigned int i = 0; i < group->references.size(); i++)
			{
				GameObjectReference* reference = group->references[i];
				if (!reference->cullingFlag)
				{
					reference->draw(pass);
					drawCount++;
				}
			}
		}
	}
	sceneObjectsLock.unlock();
}

void Scene::addObjectReference(GameObjectReference* reference)
{
	//check to see if there is an object group for this object already
	//need to set this up so that it is sorted for quick lookups
	totalObjects++;
	GameObject* baseObject = reference->base;
	for (auto& group : objectsInScene)
	{
		if (group->gameObject == baseObject)
		{
			group->references.push_back(reference);
			return;
		}
	}
	//if we've made it down to here, there was no object group for this base object
	ObjectAndReferencesGroup* group = new ObjectAndReferencesGroup;
	group->gameObject = baseObject;
	group->references.push_back(reference);
	objectsInScene.push_front(group);
}

void Scene::deleteReferencesByTag(int tag)
{
	sceneObjectsLock.lock();
	for (auto& group : objectsInScene)
	{
		for (auto i = group->references.begin(); i != group->references.end();)
		{
			GameObjectReference* p = *i;
			if (p->tag == tag)						//Ran into a crash here when moving out into the water
			{
				totalObjects--;
				i = group->references.erase(i);
				delete p;
			}
			else
			{
				i++;
			}
		}
	}
	sceneObjectsLock.unlock();
}

void Scene::addObjectReferenceBatch(std::vector<GameObjectReference*> batch)
{
	sceneObjectsLock.lock();
	for (unsigned int i = 0; i < batch.size(); i++)
	{
		addObjectReference(batch[i]);		//this can be massively optimized
	}
	sceneObjectsLock.unlock();
}