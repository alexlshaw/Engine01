#ifndef ENGINE01_OCEANRENDERER_H
#define ENGINE01_OCEANRENDERER_H

#include "glm\glm.hpp"
#include "glm\gtx\transform.hpp"
#include <vector>

#include "Camera.h"
#include "DebuggingTools.h"
#include "Settings.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Vertex.h"

//Class for rendering the ocean
//for now, much of this is a duplicate of terrainRenderer

class OceanRenderer
{
private:
	Shader ocean;
	GLuint modelMatrix, viewMatrix, projectionMatrix, texUniform;
	Texture waterTex;
	GLuint rVBO, rVAO, rIBO;
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;
	int totalVertices, totalIndices;
	int gridSnap;
	int meshRadius;

	void buildMeshSegment(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int outerRadius, int quadSize);
	void buildMeshSegmentSided(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int outerRadius, int quadSize);
	void buildMeshSegmentSkirt(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int quadSize);
	void buildMeshSegmentSkirtSided(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int quadSize);
	void buildRectangularSegment(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int startingX, int startingZ, int xSize, int zSize, int quadSize);
	void addTriangle(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float quadSize);
	void pushMesh(std::vector<SimpleVertex> vertices, std::vector<unsigned int> indices);
	void generateMesh();
	void loadShader();
	Transform transform;
public:
	OceanRenderer();
	~OceanRenderer();
	void draw(Camera* camera);
	void updateTransform(Camera* camera);
};

#endif