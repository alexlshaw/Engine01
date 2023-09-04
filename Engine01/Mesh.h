#ifndef ENGINE01_MESH_H
#define ENGINE01_MESH_H

#include <fstream>
#include <vector>
#include <string>

#include "DebuggingTools.h"
#include "glad/glad.h"
#include "Vertex.h"

class Mesh
{
private:
	GLuint vbo, vao, ibo;
	int indexCount;
	void countObj(std::ifstream& input, unsigned int& vertexPositionCount, unsigned int& vertexUVCount, unsigned int& vertexNormalCount, unsigned int& indexCount);
public:
	Mesh();
	Mesh(std::vector<SimpleVertex> vertices, std::vector<unsigned int> indices);
	Mesh(std::vector<ColouredVertex> vertices, std::vector<unsigned int> indices);
	void LoadFromObj(std::string fileName);
	~Mesh();
	void draw();
};

#endif