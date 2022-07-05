#ifndef ENGINE01_SKYBOX_H
#define ENGINE01_SKYBOX_H

#include <gl\glew.h>
#include "glm\glm.hpp"
#include <vector>

#include "Camera.h"
#include "GraphicsResourceManager.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "Shader.h"
#include "Vertex.h"


//class for drawing the sky. Should be owned by a scene (the scene draws the skybox, then
//the contents of the scene.)
//the basic idea of the skybox is that it renders a cube (or any other wrapping shape)
//which is the sky itself, then clears the depth buffer, then draws any sky objects 
//like clouds or stars, and the sun/moon
//then clears the depth buffer again, ready for the scene to be drawn

//the outer box takes two parameters - the colour of the sky at the horizon, and the colour
//of the sky at the top
class Skybox
{
private:
	Mesh* skyboxOuter;
	Mesh* clouds;
	Mesh* sunMesh;
	Texture* sunTex;
	Shader* skyShader;
	GLuint skyShader_horizon, skyShader_top, skyShader_view, skyShader_projection, skyShader_sunPos;
	Shader* sunShader;
	GLuint sunShader_view, sunShader_projection, sunShader_sunPosition, sunShader_tex;
	glm::vec3 sunPosition;
	//Mesh* stars;
	//Mesh* moon;
	void buildMeshes();
	void loadShaders(GraphicsResourceManager* resourceManager);
public:
	Skybox(GraphicsResourceManager* resourceManager, glm::vec3 sunPosition);
	~Skybox();
	void draw(Camera* camera);
	void updateSunPosition(glm::vec3 newPosition);
};

#endif