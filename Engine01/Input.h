#ifndef ENGINE01_INPUT_H
#define ENGINE01_INPUT_H

#include "SDL\SDL.h"

#include "Camera.h"
#include "Settings.h"
#include "WorldMap.h"

extern bool newEvent;
extern bool exiting;
extern bool debugMode;
extern WorldMap* worldMap;

//depending on how many of these I need, I may just use an array
static bool keyFLastDownState = false;
static bool keyTabLastDownState = false;
static bool keyMLastDownState = false;
static bool keyKLastDownState = false;
static bool keyOLastDownState = false;
static bool keyRLastDownState = false;
static bool keyNLastDownState = false;

void handleKeys(float delta, Camera* cam);
void handleMouse(float delta, SDL_Event event, Camera* cam);

#endif