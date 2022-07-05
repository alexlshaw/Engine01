#include <chrono>
#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glutils.h"
#include <math.h>
#include <iostream>
#include "SDL\SDL.h"
#include <stdio.h>
#include <thread>

#include "Camera.h"
#include "Cube.h"
#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include "GrassRenderer.h"
#include "Input.h"
#include "Math.h"
#include "MeshTools.h"
#include "Noise.h"
#include "OceanRenderer.h"
#include "Rock.h"
#include "Scene.h"
#include "Settings.h"
#include "Skybox.h"
#include "Sphere.h"
#include "StopWatch.h"
#include "TaskManager.h"
#include "TerrainRenderer.h"
#include "Texture.h"
#include "Text2d.hpp"
#include "Time.h"
#include "Tree.h"
#include "UIManager.h"
#include "World.h"
#include "WorldMap.h"

//Declare display and control variables
SDL_Window* window;
SDL_GLContext glContext;
SDL_Event event;
bool exiting = false;
bool newEvent = false;
TaskManager* taskManager;
GraphicsResourceManager* resourceManager;
UIManager* uiManager;
int screenWidth, screenHeight;
int frames = 0;
int fps = 0;

bool wireframe = false;
bool debugMode = false;

int mousePosX;
int mousePosY;
int lastMouseX = screenWidth / 2;
int lastMouseY = screenHeight / 2;

int minScreenDimension;

//display stuff variables
Camera* mainCamera;
glm::mat4 orthoProjection;
glm::vec3 startPosition = vec3(9500.0f, 5.0f, 4100.0f);
//glm::vec3 startPosition = vec3(300.0f * FWORLD_POINT_SEPARATION, 5.0f, 75.0f * FWORLD_POINT_SEPARATION);
//glm::vec3 startPosition = vec3(1.0f, 5.0f, 1.0f);
float fallingVelocity = 0.0f;
WorldMap* worldMap;

//terrain variables
World *mainWorld;
TerrainRenderer* terrainRenderer;
OceanRenderer* oceanRenderer;
GrassRenderer* grassRenderer;
Scene* mainScene;
Skybox* sky;

bool initTest()
{
	//Cube* positionHolder = new Cube(resourceManager->loadMaterial("DefaultBark"));
	//GameObjectReference* cubeRef = new GameObjectReference(positionHolder);
	//cubeRef->transform->setPosition(startPosition);
	//mainScene->addObjectReference(cubeRef);

	//Sphere* testSphere = new Sphere(resourceManager->loadMaterial("BarkLit"));
	//GameObjectReference* sphereRef = new GameObjectReference(testSphere);
	//sphereRef->transform->setPosition(glm::vec3(startPosition.x, startPosition.y - 3.0f, startPosition.z));
	//mainScene->addObjectReference(sphereRef);

	//Rock* testRock = new Rock(resourceManager->loadMaterial("RockLit"));
	//GameObjectReference* rockRef = new GameObjectReference(testRock);
	//rockRef->transform->setPosition(glm::vec3(startPosition.x + 10.0f, startPosition.y - 20.0f, startPosition.z));
	//mainScene->addObjectReference(rockRef);
	return true;
}

int exit()
{
	//Destroy the task manager
	delete taskManager;		//It is critical that taskManager be destroyed before anything that may have queued tasks on it
	taskManager = nullptr;	//so that the worker threads don't find themselves working on objects that have been destroyed
	//destroy the other stuff
	delete sky;
	sky = nullptr;
	delete mainScene;
	mainScene = nullptr;
	cleanupText2D();
	delete mainWorld;
	mainWorld = nullptr;
	delete terrainRenderer;
	terrainRenderer = nullptr;
	delete grassRenderer;
	grassRenderer = nullptr;
	delete oceanRenderer;
	oceanRenderer = nullptr;
	delete uiManager;
	uiManager = nullptr;
	delete resourceManager;
	resourceManager = nullptr;
	SDL_Quit();
	//DEBUG_WAIT_FOR_CONSOLE_INPUT();	//Uncomment to allow all console output to be seen before the program exits
	return 0;
}

bool initGL()
{
	//glew
	DEBUG_PRINT("Init glew\n");
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		DEBUG_PRINT("Error initialising glew\n");
		return false;
	}
	DEBUG_PRINT("GlewInit: Expected warning GL_INVALID_ENUM\n");
	printGLErrors();
	DEBUG_PRINT("Init glew finished\n");
	//gl stuff
	DEBUG_PRINT("Init GL\n");
	glClearColor(fogColor.r, fogColor.g, fogColor.b, 1.0f);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);	
	glEnable(GL_PROGRAM_POINT_SIZE);
	//back to new stuff
	if (glGetError() == GL_NO_ERROR)
	{
		glActiveTexture(GL_TEXTURE0);
		DEBUG_PRINT("Init GL finished\n");
		return true;
	}
	return false;
}

bool initSDL()
{
	DEBUG_PRINT("Init SDL\n");
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	Uint32 windowFlags = SDL_WINDOW_OPENGL;

	if (fullScreen)
	{
		screenWidth = fullScreenWidth;
		screenHeight = fullScreenHeight;
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}
	else
	{
		screenWidth = windowedScreenWidth;
		screenHeight = windowedScreenHeight;
	}
	lastMouseX = screenWidth / 2;
	lastMouseY = screenWidth / 2;
	orthoProjection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
	window = SDL_CreateWindow("Engine 01", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, windowFlags);
	if (window == NULL)
	{
		return false;
	}
	glContext = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	DEBUG_PRINT_GL_ERRORS();
	DEBUG_PRINT("Init SDL finished\n");
	return true;
}

void draw()
{
	mainCamera->calculateViewMatrix();
	mainCamera->calculateViewMatrixNoPosition();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//draw the background
	sky->draw(mainCamera);
	//draw the terrain
	terrainRenderer->draw(mainCamera, mainWorld);
	//draw the grass
	grassRenderer->draw(mainCamera, mainWorld);
	//draw the other objects in the scene
	mainScene->setCullList();
	mainScene->draw();
	//draw the water
	oceanRenderer->draw(mainCamera);
	
	//draw the user interface
	uiManager->drawInterface(orthoProjection);
	
	//draw the debug info
	if (debugMode)
	{
		char camPosition[64];
		char camAngle[64];
		char sceneInfo[128];
		
		mainCamera->getPositionString(camPosition);
		mainCamera->getAngleString(camAngle);
		sprintf_s(sceneInfo, 128, "Scene drawing %i object passes, %i material activations. %i total objects. %i FPS", mainScene->drawCount, mainScene->materialActivations, mainScene->totalObjects, fps);
		printText2D(camPosition, 15, screenHeight - 20, 15);
		printText2D(camAngle, 15, screenHeight - 40, 15);
		printText2D(sceneInfo, 15, screenHeight - 60, 15);
	}
	SDL_GL_SwapWindow(window);
}

void update(float delta)
{
	//make sure the camera does not stray out of the bounds of the world
	if (mainCamera->position.x < 0.0f)
	{
		mainCamera->position.x = 0.0f;
	}
	else if (mainCamera->position.x > (float)(WORLD_WIDTH) - 0.1f)
	{
		mainCamera->position.x = (float)(WORLD_WIDTH)-0.1f;
	}
	if (mainCamera->position.z < 0.0f)
	{
		mainCamera->position.z = 0.0f;
	}
	else if (mainCamera->position.z >(float)(WORLD_WIDTH)-0.1f)
	{
		mainCamera->position.z = (float)(WORLD_WIDTH)-0.1f;
	}

	if (!mainCamera->floating)
	{
		float desiredHeight = EYE_HEIGHT + mainWorld->getTerrainHeightAtPoint(mainCamera->position.x, mainCamera->position.z);
		if (mainCamera->position.y > desiredHeight)
		{
			mainCamera->position.y -= 0.5f * GRAVITY * delta * delta + delta * fallingVelocity;
			fallingVelocity += GRAVITY * delta;
		}
		else
		{
			mainCamera->position.y = desiredHeight;
			fallingVelocity = 0.0f;
		}
	}
	mainWorld->update(mainCamera);
}

bool init(CStopWatch* timer)
{
	//returns true if both parts suceeded
	bool initSDLsuccess = initSDL();
	if (!initSDLsuccess)
	{
		DEBUG_PRINT("Failed to initialise SDL\n");
	}
	bool initGLsuccess = initGL();
	if (!initGLsuccess)
	{
		DEBUG_PRINT("Failed to initialise OpenGL\n");
	}
	const GLubyte* version = glGetString(GL_VERSION);
	if (initSDLsuccess && initGLsuccess && glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		resourceManager = new GraphicsResourceManager();
		uiManager = new UIManager(screenWidth, screenHeight, resourceManager);
		mainCamera = new Camera(startPosition, vec3(0.0f, 0.0f, 0.5f), true, screenWidth, screenHeight);
		initText2D(screenWidth, screenHeight); 
		initSimplexNoise();
		srand(worldSeed);
		RandomInit(worldSeed);
		sky = new Skybox(resourceManager, lightPos);
		mainScene = new Scene(mainCamera);
		taskManager = new TaskManager();

		DEBUG_PRINT("Beginning World generation\n");
		mainWorld = new World(taskManager, mainScene, resourceManager);
		char worldGenTime[32];
		sprintf_s(worldGenTime, 32, "Worldgen time: %f\n", timer->GetElapsedSeconds());
		DEBUG_PRINT(worldGenTime);

		worldMap = mainWorld->generateWorldMap();
		uiManager->addElement(worldMap);
		//build the terrain renderer objects
		terrainRenderer = new TerrainRenderer(resourceManager);
		grassRenderer = new GrassRenderer(resourceManager);
		oceanRenderer = new OceanRenderer();

		initTest();
		return true;
	}
	else
	{
		DEBUG_PRINT("Bad Framebuffer status, exiting\n");
		return false;
	}
}

int main(int argc, char* args[])
{
	//initialise everything
	static CStopWatch fpsTimer;
	static CStopWatch timer;
	exiting = !init(&timer);
	char startupTime[32];
	sprintf_s(startupTime, 32, "Total startup time: %f\n", timer.GetElapsedSeconds());
	DEBUG_PRINT(startupTime);
	timer.Reset();
	while (!exiting)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) exiting = true;
			newEvent = true;
		}
		float delta = timer.GetElapsedSeconds();
		timer.Reset();
		//handle logic updates
		update(delta);
		//handle input
		handleKeys(delta, mainCamera);
		handleMouse(delta, event, mainCamera);
		//handle in game time based events
		timePassing(delta);
		//draw everything
		draw();
		frames++;
		newEvent = false;
		//No point running at a higher framerate than 60 for the time being, so we give the computer a bit of a break if there is any time left
		delta = timer.GetElapsedSeconds() * 1000.0f;
		if (delta < 16.6f)
		{
			long sleep = (long)(16.6f - delta);
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
		}
		if (fpsTimer.GetElapsedSeconds() > 1.0f)
		{
			fps = frames;
			frames = 0;
			fpsTimer.Reset();
		}
	}
	//Clean up resources and exit
	return exit();
}