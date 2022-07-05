#ifndef ENGINE01_TERRAINRENDERER_H
#define ENGINE01_TERRAINRENDERER_H

//This class represents a moderately complex operation
//it stores a mesh that is attached to the camera
//such that the camera is always looking out over the mesh
//This mesh is a triangle grid that decreases in density the 
//further away from the camera it gets.
//When rendered, each point in this mesh is updated to hold the
//height at the point it *currently* represents on the terrain
//this is achieved by looking at heightmap textures owned by the nodes in the World class

//NOTE: It might be possible to skip computation of the model matrix
//since the terrainRenderer is always positioned such that its origin
//and rotation are the same as the camera's,
//the model and view matrices should cancel each other out
//which means that as long as the final pass rendering is aware of the camera's height
//so it can adjust the final height based on how far above the terrain the camera is,
//it shoudln't need to do any more complicated transforms
//Though on further thought, while the terrain renderer's rotation around the y axis may always match the camera's
//The terrain renderer mesh should not tilt downwards when the camera does
//Additionally, it is advantageous to lock the terrain renderer mesh to the terrain grid
//otherwise, there is some slight wobbliness as the vertices change heights


#include "glm\glm.hpp"
#include <vector>

#include "Camera.h"
#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include "Settings.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Vertex.h"
#include "World.h"

class TerrainRenderer
{
private:
	Shader terrain2;
	GLuint terrain2_modelView, terrain2_projection, terrain2_transform, terrain2_heightmapTexture, terrain2_splatMaps;
	GLuint terrain2_grassTex, terrain2_dirtTex, terrain2_rockTex, terrain2_sandTex, terrain2_snowTex;
	GLuint terrain2_lightIntensity, terrain2_ambient, terrain2_diffuse, terrain2_lightPos;
	GLuint rVBO, rVAO, rIBO;
	Texture* dirtLayer;
	Texture* grassLayer;
	Texture* rockLayer;
	Texture* sandLayer;
	Texture* snowLayer;
	GraphicsResourceManager* resourceManager;
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
	void loadShader();
	void generateMesh();
	Transform transform;
public:
	TerrainRenderer(GraphicsResourceManager* resourceManager);
	~TerrainRenderer();
	
	void updateTransform(Camera* camera);
	void draw(Camera* camera, World* world);
};

#endif