#ifndef ENGINE01_VERTEX_H
#define ENGINE01_VERTEX_H

#include "glad/glad.h"
#include "glm\glm.hpp"

struct SimpleVertex			//simplevertex is used by the terrain renderer and only tracks position
{
	glm::vec3 position;
	SimpleVertex(glm::vec3 pos) { position = pos; }
};

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

struct ColouredVertex
{
	glm::vec4 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec4 color;
	GLubyte padding[12];	//pad to 64 bytes -> improved performance on some cards
};

struct LinkedVertex			//Structure required for wireframe drawing
{
	glm::vec4 position;		//xyzw, where w contains the vertex index, 0, 1, or 2
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec4 color;
	glm::vec3 p1_3d;		//The first of the other two verticies in the triangle
	glm::vec3 p2_3d;		//The second of the other two vertices in the triangle
};

inline void setSimpleVertexAttribs()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
}

inline void setVertexAttribs()
{
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
	//texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	glEnableVertexAttribArray(1);
	//normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
	glEnableVertexAttribArray(2);
}

inline void setColouredVertexAttribs()
{
	//position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
	//texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	//normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const GLvoid*)24);
	glEnableVertexAttribArray(2);
	//color
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const GLvoid*)36);
	glEnableVertexAttribArray(3);
}

inline void setLinkedVertexAttribs()
{
	//Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(LinkedVertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
	//texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(LinkedVertex), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	//normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(LinkedVertex), (const GLvoid*)24);
	glEnableVertexAttribArray(2);
	//colour
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(LinkedVertex), (const GLvoid*)36);
	glEnableVertexAttribArray(3);
	//Pointers to the other vertex locations in the polygon
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(LinkedVertex), (const GLvoid*)52);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(LinkedVertex), (const GLvoid*)64);
	glEnableVertexAttribArray(5);
}


#endif