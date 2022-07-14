#ifndef ENGINE01_MESH_H
#define ENGINE01_MESH_H

#include "glad/glad.h"
#include <vector>

#include "Vertex.h"

class Mesh
{
private:
	GLuint vbo, vao, ibo;
	int indexCount;
public:
	Mesh(std::vector<SimpleVertex> vertices, std::vector<unsigned int> indices);
	Mesh(std::vector<ColouredVertex> vertices, std::vector<unsigned int> indices);
	~Mesh();
	void draw();
};

#endif