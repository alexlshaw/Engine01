#include <chrono>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glutils.h"
#include <math.h>
#include <iostream>
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
GLFWwindow* mainWindow = nullptr;

bool exiting = false;
bool newEvent = false;
TaskManager* taskManager;
GraphicsResourceManager* resourceManager;
UIManager* uiManager;
int screenWidth, screenHeight;
int frames = 0;
int fps = 0;
float delta = 0.0f;	//time since last frame

bool wireframe = false;
bool debugMode = false;

int mousePosX;
int mousePosY;
double lastMouseX = (double)(screenWidth / 2);
double lastMouseY = (double)(screenHeight / 2);

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


//glfw callbacks
void error_callback(int error, const char* description)
{
	fprintf_s(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool keyDown = action == GLFW_PRESS || action == GLFW_REPEAT;
	switch (key)
	{
	case GLFW_KEY_W:
		keyWState = keyDown;
		break;
	case GLFW_KEY_S:
		keySState = keyDown;
		break;
	case GLFW_KEY_A:
		keyAState = keyDown;
		break;
	case GLFW_KEY_D:
		keyDState = keyDown;
		break;
	case GLFW_KEY_SPACE:
		keySpaceState = keyDown;
		break;
	case GLFW_KEY_LEFT_SHIFT:
		keyLShiftState = keyDown;
		break;
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		mainCamera->floating = !mainCamera->floating;
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		worldMap->visible = !worldMap->visible;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		worldMap->cycleActiveMapType();
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		debugMode = !debugMode;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		velocity = velocity == walkingSpeed ? boostSpeed : walkingSpeed;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		exiting = true;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	double deltaX = lastMouseX - xpos;
	double deltaY = lastMouseY - ypos;
	lastMouseX = xpos;
	lastMouseY = ypos;
	mainCamera->updateDirection((float)deltaX, (float)deltaY);
}

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
	glfwTerminate();
	//DEBUG_WAIT_FOR_CONSOLE_INPUT();	//Uncomment to allow all console output to be seen before the program exits
	return 0;
}

bool initGL()
{
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

int initGLFW()
{
	DEBUG_PRINT("Init GLFW\n");
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		printf("GLFW init error\n");
		return -1;
	}
	//Create a windowed mode window and its OpenGL context. Note: If I hint anything over 3.0 it doesn't like it, but testing shows I'm getting a 4.6 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	if (fullScreen)
	{
		screenWidth = fullScreenWidth;
		screenHeight = fullScreenHeight;
		mainWindow = glfwCreateWindow(screenWidth, screenHeight, "Engine01", glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		screenWidth = windowedScreenWidth;
		screenHeight = windowedScreenHeight;
		mainWindow = glfwCreateWindow(screenWidth, screenHeight, "Engine01", NULL, NULL);
	}
	if (!mainWindow)
	{
		glfwTerminate();
		return -2;
	}
	glfwMakeContextCurrent(mainWindow);
	//window size dependent stuff
	lastMouseX = (double)(screenWidth / 2);
	lastMouseY = (double)(screenWidth / 2);
	orthoProjection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);

	//input handlers
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(mainWindow, key_callback);
	glfwSetMouseButtonCallback(mainWindow, mouse_button_callback);
	glfwSetScrollCallback(mainWindow, scroll_callback);
	glfwSetCursorPosCallback(mainWindow, cursor_position_callback);
	glfwGetCursorPos(mainWindow, &lastMouseX, &lastMouseY);	//update lastX and lastY so first cursor movement doesn't register as hundreds of pixels
	//set up glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("GLAD loader initialization error!\n");
		return -3;
	}
	glfwSwapInterval(1);
	return 1;
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
	glfwSwapBuffers(mainWindow);
}

void update(float delta)
{
	//process camera movement
	if (keyWState)
	{
		mainCamera->forward(delta * velocity);
	}
	else if (keySState)
	{
		mainCamera->forward(delta * -velocity);
	}
	if (keyAState)
	{
		mainCamera->pan(delta * velocity);
	}
	else if (keyDState)
	{
		mainCamera->pan(delta * -velocity);
	}
	if (keySpaceState)
	{
		mainCamera->rise(delta * velocity);
	}
	else if (keyLShiftState)
	{
		mainCamera->rise(delta * -velocity);
	}

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
	int initGLFWsuccess = initGLFW();
	if (initGLFWsuccess < 1)
	{
		DEBUG_PRINT("Failed to initialise GLFW\n");
	}
	bool initGLsuccess = initGL();
	if (!initGLsuccess)
	{
		DEBUG_PRINT("Failed to initialise OpenGL\n");
	}
	const GLubyte* version = glGetString(GL_VERSION);
	if ((initGLFWsuccess == 1) && initGLsuccess && glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
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
	while (!exiting && !glfwWindowShouldClose(mainWindow))
	{
		delta = timer.GetElapsedSeconds();
		timer.Reset();
		//handle logic updates
		update(delta);
		//handle input
		//handleKeys(delta, mainCamera);
		//handleMouse(delta, mainCamera);
		//handle in game time based events
		timePassing(delta);
		//draw everything
		draw();
		frames++;
		glfwPollEvents();
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