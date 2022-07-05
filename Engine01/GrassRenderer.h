#ifndef ENGINE01_GRASSRENDERER_H
#define ENGINE01_GRASSRENDERER_H

#include "gl/glew.h"
#include "glm/glm.hpp"

#include "Camera.h"
#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include "Shader.h"
#include "Texture.h"
#include "World.h"

//class for drawing a grass mesh that moves with the player
class GrassRenderer
{
private:
	Shader* grass;
	GLuint grass_modelView, grass_projection, grass_transform, grass_textures;
	GLuint grass_heightMapTexture, grass_splatMapTexture;
	GLuint rVBO, rVAO, rIBO;
	Texture* grasses;
	Transform transform;
	int totalVertices, totalIndices;
	void loadShader(GraphicsResourceManager* resourceManager);
	void generateMesh();
	void pushMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	void addTuft(float x, float y, float z, std::vector<Vertex>* vertices, std::vector<unsigned int>* indices);
public:
	GrassRenderer(GraphicsResourceManager* resourceManager);
	~GrassRenderer();

	void updateTransform(Camera* camera);
	void draw(Camera* camera, World* world);
};

#endif