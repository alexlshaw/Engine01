#ifndef ENGINE01_SCENE_H
#define ENGINE01_SCENE_H

#include <forward_list>
#include <mutex>
#include <vector>

#include "Camera.h"
#include "GameObject.h"
#include "GameObjectReference.h"
#include "Settings.h"

struct ObjectAndReferencesGroup
{
	GameObject* gameObject;
	std::vector<GameObjectReference*> references;
};

//this class exists to store object references for the purposes of drawing them.
class Scene
{
private:
	Camera* mainCamera;
	std::forward_list<ObjectAndReferencesGroup*> objectsInScene;
	std::mutex sceneObjectsLock;
public:
	int totalObjects;
	int drawCount;
	int materialActivations;
	Scene(Camera* mainCamera);
	~Scene();
	void setCullList();
	void draw();
	void addObjectReference(GameObjectReference* reference);	//not thread safe
	void addObjectReferenceBatch(std::vector<GameObjectReference*> batch);	//thread safe
	void deleteReferencesByTag(int tag);
};

#endif