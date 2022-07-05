#ifndef ENGINE01_TRANSFORM_H
#define ENGINE01_TRANSFORM_H

#include "glm\glm.hpp"
#include "glm\gtx\transform.hpp" 
#include "glm\gtc\matrix_inverse.hpp"

class Transform
{
private:
	glm::vec3 position;
	glm::vec4 rotation;				//TODO: handle this in a real manner (quaternion?), instead of vec3 rotation axis + 4th param degrees ...maybe...
	glm::vec3 scale;
	glm::mat4 transform;
	glm::mat3 normal;
	bool dirty;						//set whenever one of the individual properties is changed
	void buildTransformMatrix();
public:
	Transform();
	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);
	glm::vec3 getScale();
	void setScale(glm::vec3 scl);
	glm::vec4 getRotation();
	void setRotation(glm::vec4 rot);
	glm::mat4 getMatrix();
	glm::mat3 getNormalMatrix();
};

#endif