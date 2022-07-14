#include "Camera.h"

Camera::Camera()
{
	position = glm::vec3();
	direction = glm::vec3(0.0f, 0.0f, 0.5f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	floating = false;
	horizontalAngle = 90.0f;
	verticalAngle = 90.0f;
	aspectRatio = 1.0f;
	projectionMatrix = glm::identity<glm::mat4>();
	viewMatrix = glm::identity<glm::mat4>();
	viewMatrixOrigin = glm::identity<glm::mat4>();
}

Camera::Camera(glm::vec3 pos, glm::vec3 dir, bool isFloating, int screenWidth, int screenHeight)
{
	position = pos;
	direction = dir;
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	floating = isFloating;
	horizontalAngle = 90.0f;
	verticalAngle = 90.0f;
	aspectRatio = float(screenWidth) / float(screenHeight);
	frustrum.setCamInternals(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);
	frustrum.setCamDef(position, position + direction, up);
	calculateProjectionMatrix(screenWidth, screenHeight);
}

Camera::~Camera()
{
}

//Moves the camera along the (camera's) x axis
void Camera::pan(float delta)
{
	//position -= right * delta;	//This is a cleaner way of doing it, but until I sort the other camera problems out I don't want to worry about defining right

	float moveX, moveZ;
	float tempAngle = horizontalAngle - 90.0f;
	//the camera's angle->y determines which direction we are facing on the x-z plane
	moveX = (float)(cos(tempAngle * DEGREES_TO_RADIANS));
	moveZ = (float)(sin(tempAngle * DEGREES_TO_RADIANS));
	float deltaX = moveX * delta;
	float deltaZ = moveZ * delta;
	position.x += deltaX;
	position.z += deltaZ;
}

//Moves the camera along the (camera's) z axis
void Camera::forward(float delta)
{
	glm::vec3 horizontalDirection = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
	position += horizontalDirection * delta;
}

//Moves the camera up or down. Not based on cameras rotation
void Camera::rise(float delta)
{
	position.y += delta;
}

void Camera::updateDirection(float dx, float dy)
{
	//pitch
	float newAngle = verticalAngle - dy;
	//first make sure the angle is in the 0 - 360 range
	if (newAngle <= 0.0)
	{
		newAngle += 360.0;
	}
	else if (newAngle >= 360.0)
	{
		newAngle -= 360.0;
	}
	//then make sure that they have not looked further from level than just shy of vertical
	//That is to say, no higher than 1 degree and no lower than 179 degrees
	if (newAngle > 270.0f)
	{
		verticalAngle = 1.0f;
	}
	else if (newAngle > 179.0f && newAngle <= 270.0f)
	{
		verticalAngle = 179.0f;
	}
	else
	{
		verticalAngle = newAngle;
	}

	//yaw
	horizontalAngle -= dx;
	if (horizontalAngle < 0.0)
	{
		horizontalAngle += 360.0;
	}
	else if (horizontalAngle >= 360.0)
	{
		horizontalAngle -= 360.0;
	}

	//Direction : Spherical coordinates to Cartesian coordinates conversion
	//theta vertical, phi horizontal
	float cartesianX = cos(horizontalAngle * DEGREES_TO_RADIANS) * sin(verticalAngle * DEGREES_TO_RADIANS);
	float cartesianY = cos(verticalAngle * DEGREES_TO_RADIANS);
	float cartesianZ = sin(horizontalAngle * DEGREES_TO_RADIANS) * sin(verticalAngle * DEGREES_TO_RADIANS);
	direction = glm::vec3(cartesianX, cartesianY, cartesianZ);

	//right = vec3(
	//	sin(horizontalAngle - 3.14f/2.0f), 
	//	0,
	//	cos(horizontalAngle - 3.14f/2.0f)
	//	);

	//up = cross( right, direction );
	up = glm::vec3(0.0f, 1.0f, 0.0f);	//for now, up is always vertical
}

void Camera::updateFrustrum()
{
	//glm::vec3 rightVector = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
	//glm::vec3 trueUp = glm::normalize(glm::cross(rightVector, direction));
	frustrum.setCamDef(position, position + direction, up);// trueUp);
}

void Camera::calculateProjectionMatrix(int screenWidth, int screenHeight)
{
	// Projection matrix : 45° Field of View, aspect ratio determined by resolution, display range : 0.1 unit <-> 2000 units
	projectionMatrix = glm::perspective(45.0f, float(screenWidth) / float(screenHeight), nearPlaneDistance, farPlaneDistance); //for when the view angle gradually changes
}

void Camera::calculateViewMatrix()
{
	viewMatrix = glm::lookAt(
		position,				// Camera is here
		position+direction,		// and looks here : at the same position, plus "direction"
		up);					// Head is up (set to 0,-1,0 to look upside-down)
		
}

void Camera::calculateViewMatrixNoPosition()
{
	//same matrix, but assumes the camera is located at the origin
	viewMatrixOrigin = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 0.0f),        // Camera is here
		direction,							// and looks here
		up);								// Head is up (set to 0,-1,0 to look upside-down)	
}

glm::mat4 Camera::getViewMatrix()
{
	return viewMatrix;
}

glm::mat4 Camera::getViewMatrixNoPosition()
{
	return viewMatrixOrigin;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

int Camera::getLookDirection()
{
	float f = atan2(direction.x, direction.z);
	int result = NORTHEAST;
	if (f >= 0.0f && f < PI / 2)
	{
		result = NORTHWEST;
	}
	else if (f >= PI / 2 && f < 2 * PI)
	{
		result = SOUTHWEST;
	}
	else if (f >= -2 * PI && f < -PI / 2)
	{
		result = SOUTHEAST;
	}

	return result;
}

void Camera::getPositionString(char* buffer)
{
	sprintf_s(buffer, 64, "x:%f, y:%f, z:%f", position.x, position.y, position.z);
}

void Camera::getAngleString(char* buffer)
{
	char dirStr[17];
	int direction = getLookDirection();
	if (direction == NORTHEAST)
	{
		sprintf_s(dirStr, "Facing Northeast");
	}
	else if (direction == NORTHWEST)
	{
		sprintf_s(dirStr, "Facing Northwest");
	}
	else if (direction == SOUTHEAST)
	{
		sprintf_s(dirStr, "Facing Southeast");
	}
	else if (direction == SOUTHWEST)
	{
		sprintf_s(dirStr, "Facing Southwest");
	}
	sprintf_s(buffer, 64, "vert: %f, hor: %f, %s", verticalAngle, horizontalAngle, dirStr);
}