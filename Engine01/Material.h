#ifndef ENGINE01_MATERIAL_H
#define ENGINE01_MATERIAL_H

#include <map>

#include "glad/glad.h"

#include "Camera.h"
#include "Settings.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

class Material
{
private:
	int modelMatrix;
	int projectionViewMatrix;
	int normalMatrix;
	int textureUniform;
	int ambientUniform;
	int diffuseUniform;
	int lightIntensityUniform;
	int lightPositionUniform;
	Shader* shader;
	Texture* texture;		//may need to improve to handle multiple textures for one material
	bool lit;
public:
	Material(Shader* shader, Texture* texture);
	void setLit(bool val);
	void use(Camera* camera);
	void setTransform(Transform* transform);
	bool enableBlending;
};

#endif