#ifndef ENGINE01_CAMERA_H
#define ENGINE01_CAMERA_H

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "DebuggingTools.h"
#include "Frustrum.h"
#include <math.h>
#include "Math.h"
#include "Settings.h"
#include "Transform.h"

class Camera
{
private:
	glm::mat4 viewMatrix;
	glm::mat4 viewMatrixOrigin;
	glm::mat4 projectionMatrix;
	glm::vec3 up;

	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 2000.0f;
	float fieldOfView = 45.0f;
	float aspectRatio;
public:
	glm::vec3 position;
	glm::vec3 direction;
	float horizontalAngle;
	float verticalAngle;

	Camera();
	Camera(glm::vec3 pos, glm::vec3 dir, bool, int screenWidth, int screenHeight);
	~Camera();
	void pan(float delta);
	void rise(float delta);
	void forward(float delta);
	void updateDirection(float dx, float dy);
	void updateFrustrum();	//call this after done adjusting camera position/direction for a frame
	void calculateViewMatrix();
	void calculateViewMatrixNoPosition();
	void calculateProjectionMatrix(int screenWidth, int screenHeight);
	glm::mat4 getViewMatrix();
	glm::mat4 getViewMatrixNoPosition();
	glm::mat4 getProjectionMatrix();

	int getLookDirection();
	void getPositionString(char* buffer);
	void getAngleString(char* buffer);

	bool floating;
	bool facing(Transform transform);
	Frustrum frustrum;
};

#endif