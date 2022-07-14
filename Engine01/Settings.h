#ifndef ENGINE01_SETTINGS_H
#define ENGINE01_SETTINGS_H

/*---------------------------------------------
Contains all the magic numbers which control the application
---------------------------------------------*/

#include "glm\glm.hpp"

//ENUMS
const enum lookDirection {SOUTHWEST, NORTHWEST, NORTHEAST, SOUTHEAST};
const enum frustrumIntersection { OUTSIDE, INTERSECT, INSIDE };
const static int SPLAT_SAND = 0;
const static int SPLAT_DIRT = 1;
const static int SPLAT_GRASS = 2;
const static int SPLAT_SNOW = 3;

//terrain renderer system variables
const static int HEIGHTMAP_TEXTURE_WIDTH = 1024;
static int NODES_PER_WORLD_SIDE = 16;
static int WORLD_WIDTH = HEIGHTMAP_TEXTURE_WIDTH * NODES_PER_WORLD_SIDE;		//width of the world in verticies
static int TOTAL_NODES = NODES_PER_WORLD_SIDE * NODES_PER_WORLD_SIDE;
static int NOISEMAP_X = 970;	//arbitrary value: larger means less repetition, but beyond a point it's going to be invisible anyway
static int NOISEMAP_Z = 955;	//arbitrary value
static float FNOISEMAP_X = (float)NOISEMAP_X;
static float FNOISEMAP_Z = (float)NOISEMAP_Z;

static int WORLD_POINT_SEPARATION = 32;	//how far apart the GeoPoints and ClimatePoints are
static float FWORLD_POINT_SEPARATION = (float)WORLD_POINT_SEPARATION;
static int WORLD_MAP_TEXTURE_WIDTH = WORLD_WIDTH / WORLD_POINT_SEPARATION;
static int WORLD_POINTS_PER_SIDE = (WORLD_WIDTH / WORLD_POINT_SEPARATION) + 1;
static float FWORLD_MAP_TEXTURE_WIDTH = (float)(WORLD_MAP_TEXTURE_WIDTH);

//world construction parameters
const static float ISLAND_FALLOFF = 0.6f;	//the higher this is, the more strongly distance from the centre of the map affects whether a cell is water or not
const static float COAST_NOISE_FREQUENCY = 0.015f;
const static float WATER_THRESHOLD_BASE = 0.3f;	//the higher this is, the more likely a cell is to be land


const static float SEA_LEVEL = 0.0f;	//The height below which is considered to be under the sea

static float walkingSpeed = 10.0f;		//how fast you move when walking
static float boostSpeed = 150.0f;		//how fast you move when boosting
static float velocity = walkingSpeed;	//the distance you should move in a second
static float rotationSpeed = 360.0f;	//the number of degrees by which your view should be able to rotate in a second
const float EYE_HEIGHT = 1.8f;			//how high the camera is above the terrain
const float GRAVITY = 9.81f;			//how fast falling accellerates

//Graphics Settings
static bool fullScreen = false;
static int windowedScreenWidth = 1024;
static int windowedScreenHeight = 768;
static int fullScreenWidth = 1920;
static int fullScreenHeight = 1080;
const static int SCREEN_BPP = 32;
const static int DESIRED_FPS = 60;

const static int TREE_SIDES = 8;		//The number of sides a cylindrical portion of a tree has
const static int TREE_VARIANTS = 4;	//the number of models generated for each species of tree
const static int ROCK_VARIANTS = 6;	//the number of models generated for each type of rock

static glm::vec3 skyColor = glm::vec3(0.2f, 0.2f, 1.0f);
static glm::vec3 fogColor = glm::vec3(0.8f, 0.8f, 1.0f);

static unsigned long worldSeed = 765488;

//draw distance variables
const static float VIEW_DISTANCE = 1000.0f;
const static float VIEW_SQUARED = VIEW_DISTANCE * VIEW_DISTANCE;
const static float DEFAULT_DRAW_SCALE = 0.1f; //by default, objects are only drawn up to a tenth of maximum draw distance
const static float TREE_DRAW_SCALE = 0.8f;		//the fraction of maximum draw distance trees are drawn up to
const static float ROCK_DRAW_SCALE = 0.5f;		//the fraction of maximum draw distance rocks are drawn up to

//lighting variables
static glm::vec3 Ka = glm::vec3(0.3f, 0.3f, 0.3f);
static glm::vec3 Kd = glm::vec3(1.0f, 1.0f, 1.0f);
static glm::vec3 lightPos = glm::vec3(-1000.0f, 1000.0f, -1000.0f);// vec3(0.0, 8000.0, 0.0);

#endif