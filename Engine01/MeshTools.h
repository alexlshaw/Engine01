#ifndef ENGINE01_MESHTOOLS_H
#define ENGINE01_MESHTOOLS_H

//This file (with associated .cpp) is for helper functions for procedural mesh generation: building geometric shapes and the like

#include "glm\glm.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include <vector>

#include "Noise.h"
#include "Vertex.h"

namespace MeshTools
{
	void addCube(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 center, float width);

	void addCylinder(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 center, glm::vec3 direction, float height, float radius, int sides);

	void addVertexRing(std::vector<ColouredVertex>* vertices, glm::vec3 center, glm::vec3 normal, float radius, int sides);

	//Fills in space between two vertex rings to make a tube. Assumes last (sides * 2) vertices in the buffer represent the rings
	void linkVertexRings(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, int sides);

	//Fills in space between two vertex rings to make a tube. last 1 and last 2 give the indices of the last vertex in the first and second rings, respectively
	void linkVertexRingsAtPosition(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, int sides, int last1, int last2);

	//Creates a cone by adding a point and connecting a vertex ring.
	void ringToPoint(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 point, int sides);

	//Creates a cone by adding a point and connecting a vertex ring. ringLast gives the index of the last vertex of the ring.
	void ringToPointAtPosition(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 point, int sides, int ringLast);

	//Moves a point to a random position within a certain distance along any direction
	glm::vec3 wobblePoint(glm::vec3 point, float maxDistance);

	glm::vec3 randomPointInUnitSphere();		//This method might have a clustering issue, but from my testing it seems sufficiently evenly distributed

	glm::vec2 randomPointInUnitCircle();		//This method might have a clustering issue, but from my testing it seems sufficiently evenly distributed

	void createSphere(glm::vec3 center, float radius, int rings, int sectors, std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices);
}

#endif