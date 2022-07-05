#include "MeshTools.h"

void MeshTools::addCube(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 center, float width)
{
	int currentVertexCount = vertices->size();
	float halfSize = width * 0.5f;
	glm::vec4 centrePart = glm::vec4(center.x, center.y, center.z, 0.0f);	//We use 0 so we can add it directly to the halfsize vectors and get w = 1
	ColouredVertex v1, v2, v3, v4;	//we reuse these vertices for each face of the cube
	//top side
	v1.position = glm::vec4(halfSize, halfSize, -halfSize, 1.0f) + centrePart;	v1.normal = glm::vec3(0.0f, 1.0f, 0.0f);	v1.texCoords = glm::vec2(1.0f, 0.0f);	v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v2.position = glm::vec4(halfSize, halfSize, halfSize, 1.0f) + centrePart;	v2.normal = glm::vec3(0.0f, 1.0f, 0.0f);	v2.texCoords = glm::vec2(1.0f, 1.0f);	v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v3.position = glm::vec4(-halfSize, halfSize, halfSize, 1.0f) + centrePart;	v3.normal = glm::vec3(0.0f, 1.0f, 0.0f);	v3.texCoords = glm::vec2(0.0f, 1.0f);	v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v4.position = glm::vec4(-halfSize, halfSize, -halfSize, 1.0f) + centrePart;	v4.normal = glm::vec3(0.0f, 1.0f, 0.0f);	v4.texCoords = glm::vec2(0.0f, 0.0f);	v4.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices->push_back(v1);
	vertices->push_back(v2);
	vertices->push_back(v3);
	vertices->push_back(v4);
	//left side
	v1.position = glm::vec4(halfSize, halfSize, -halfSize, 1.0f) + centrePart;	v1.normal = glm::vec3(1.0f, 0.0f, 0.0f);	v1.texCoords = glm::vec2(1.0f, 0.0f);	v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v2.position = glm::vec4(halfSize, halfSize, halfSize, 1.0f) + centrePart;	v2.normal = glm::vec3(1.0f, 0.0f, 0.0f);	v2.texCoords = glm::vec2(1.0f, 1.0f);	v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v3.position = glm::vec4(halfSize, -halfSize, halfSize, 1.0f) + centrePart;	v3.normal = glm::vec3(1.0f, 0.0f, 0.0f);	v3.texCoords = glm::vec2(0.0f, 1.0f);	v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v4.position = glm::vec4(halfSize, -halfSize, -halfSize, 1.0f) + centrePart;	v4.normal = glm::vec3(1.0f, 0.0f, 0.0f);	v4.texCoords = glm::vec2(0.0f, 0.0f);	v4.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices->push_back(v1);
	vertices->push_back(v2);
	vertices->push_back(v3);
	vertices->push_back(v4);
	//right side
	v1.position = glm::vec4(-halfSize, halfSize, -halfSize, 1.0f) + centrePart;	v1.normal = glm::vec3(-1.0f, 0.0f, 0.0f);	v1.texCoords = glm::vec2(1.0f, 0.0f);	v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v2.position = glm::vec4(-halfSize, halfSize, halfSize, 1.0f) + centrePart;	v2.normal = glm::vec3(-1.0f, 0.0f, 0.0f);	v2.texCoords = glm::vec2(1.0f, 1.0f);	v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v3.position = glm::vec4(-halfSize, -halfSize, halfSize, 1.0f) + centrePart;	v3.normal = glm::vec3(-1.0f, 0.0f, 0.0f);	v3.texCoords = glm::vec2(0.0f, 1.0f);	v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v4.position = glm::vec4(-halfSize, -halfSize, -halfSize, 1.0f) + centrePart;	v4.normal = glm::vec3(-1.0f, 0.0f, 0.0f);	v4.texCoords = glm::vec2(0.0f, 0.0f);	v4.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices->push_back(v1);
	vertices->push_back(v2);
	vertices->push_back(v3);
	vertices->push_back(v4);
	//front
	v1.position = glm::vec4(halfSize, halfSize, -halfSize, 1.0f) + centrePart;	v1.normal = glm::vec3(0.0f, 0.0f, -1.0f);	v1.texCoords = glm::vec2(1.0f, 1.0f);	v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v2.position = glm::vec4(-halfSize, halfSize, -halfSize, 1.0f) + centrePart;	v2.normal = glm::vec3(0.0f, 0.0f, -1.0f);	v2.texCoords = glm::vec2(0.0f, 1.0f);	v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v3.position = glm::vec4(-halfSize, -halfSize, -halfSize, 1.0f) + centrePart;	v3.normal = glm::vec3(0.0f, 0.0f, -1.0f);	v3.texCoords = glm::vec2(0.0f, 0.0f);	v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v4.position = glm::vec4(halfSize, -halfSize, -halfSize, 1.0f) + centrePart;	v4.normal = glm::vec3(0.0f, 0.0f, -1.0f);	v4.texCoords = glm::vec2(1.0f, 0.0f);	v4.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices->push_back(v1);
	vertices->push_back(v2);
	vertices->push_back(v3);
	vertices->push_back(v4);
	//back
	v1.position = glm::vec4(-halfSize, halfSize, halfSize, 1.0f) + centrePart;	v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v1.texCoords = glm::vec2(0.0f, 1.0f);	v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v2.position = glm::vec4(halfSize, halfSize, halfSize, 1.0f) + centrePart;	v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v2.texCoords = glm::vec2(1.0f, 1.0f);	v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v3.position = glm::vec4(halfSize, -halfSize, halfSize, 1.0f) + centrePart;	v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v3.texCoords = glm::vec2(1.0f, 0.0f);	v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v4.position = glm::vec4(-halfSize, -halfSize, halfSize, 1.0f) + centrePart;	v4.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v4.texCoords = glm::vec2(0.0f, 0.0f);	v4.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices->push_back(v1);
	vertices->push_back(v2);
	vertices->push_back(v3);
	vertices->push_back(v4);
	//bottom
	v1.position = glm::vec4(halfSize, -halfSize, -halfSize, 1.0f) + centrePart;	v1.normal = glm::vec3(0.0f, -1.0f, 0.0f);	v1.texCoords = glm::vec2(1.0f, 0.0f);	v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v2.position = glm::vec4(halfSize, -halfSize, halfSize, 1.0f) + centrePart;	v2.normal = glm::vec3(0.0f, -1.0f, 0.0f);	v2.texCoords = glm::vec2(1.0f, 1.0f);	v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v3.position = glm::vec4(-halfSize, -halfSize, halfSize, 1.0f) + centrePart;	v3.normal = glm::vec3(0.0f, -1.0f, 0.0f);	v3.texCoords = glm::vec2(0.0f, 1.0f);	v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v4.position = glm::vec4(-halfSize, -halfSize, -halfSize, 1.0f) + centrePart;	v4.normal = glm::vec3(0.0f, -1.0f, 0.0f);	v4.texCoords = glm::vec2(0.0f, 0.0f);	v4.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices->push_back(v1);
	vertices->push_back(v2);
	vertices->push_back(v3);
	vertices->push_back(v4);
	//indices
	for (int faceIndex = 0; faceIndex < 6; faceIndex++)
	{
		int vertexIndex = currentVertexCount + (4 * faceIndex);
		indices->push_back(vertexIndex);
		indices->push_back(vertexIndex + 1);
		indices->push_back(vertexIndex + 2);
		indices->push_back(vertexIndex);
		indices->push_back(vertexIndex + 2);
		indices->push_back(vertexIndex + 3);
	}
}

void MeshTools::addCylinder(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 center, glm::vec3 direction, float height, float radius, int sides)
{
	glm::vec3 normDir = glm::normalize(direction);
	//	1. calculate the center of the bottom
	glm::vec3 bottomCenter = center - (normDir * height * 0.5f);
	//	2. add a ring of vertices
	int cylinderFirstVertexIndex = vertices->size();
	MeshTools::addVertexRing(vertices, bottomCenter, normDir, radius, sides);
	//	3. fill in the ring to cap the bottom
	for (int i = cylinderFirstVertexIndex + 2; i < sides + cylinderFirstVertexIndex; i++)
	{
		indices->push_back(cylinderFirstVertexIndex);
		indices->push_back(i - 1);
		indices->push_back(i);
	}
	//	4. calculate the center of the top
	glm::vec3 topCenter = center + (normDir * height * 0.5f);
	//	5. add a ring of vertices
	int secondRingFirstVertexIndex = vertices->size();
	//MeshTools::addVertexRingFlat(vertices, topCenter, radius, sides);
	MeshTools::addVertexRing(vertices, topCenter, normDir, radius, sides);
	//	6. fill in the ring to cap the top
	for (int i = secondRingFirstVertexIndex + 2; i < sides + secondRingFirstVertexIndex; i++)
	{
		indices->push_back(secondRingFirstVertexIndex);
		indices->push_back(i - 1);
		indices->push_back(i);
	}
	//	7. link the top and the bottom
	MeshTools::linkVertexRings(vertices, indices, sides);
}

void MeshTools::addVertexRing(std::vector<ColouredVertex>* vertices, glm::vec3 center, glm::vec3 normal, float radius, int sides)
{
	float texturePerSide = 1.0f / (float)sides;
	float circumference = glm::pi<float>() * 2.0f * radius;
	glm::vec4 centerOffset = glm::vec4(center.x, center.y, center.z, 0.0f);
	//1. calculate qv1: a vector orthogonal to the normal
	glm::vec3 qv1 = glm::normalize(normal.y != 1.0f ? glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)) : qv1 = glm::cross(normal, glm::vec3(1.0f, 0.0f, 0.0f)));
	//2. for each side, rotate qv1 around the normal
	for (int i = 0; i < sides; i++)
	{
		//A: Rotate the vector
		glm::vec3 qv1Rotated = qv1;
		float angle = glm::degrees((float)i * (glm::pi<float>() * 2.0f / sides));
		qv1Rotated = glm::rotate(qv1, angle, normal) * radius;
		//B: Create a vertex
		ColouredVertex v;
		v.position = glm::vec4(qv1Rotated.x, qv1Rotated.y, qv1Rotated.z, 1.0f) + centerOffset;
		v.normal = glm::normalize(qv1Rotated);						//simplified
		v.texCoords = glm::vec2((float)i * texturePerSide, v.position.y / circumference);	//this works for vertical cylinders, but not for other ones (need to know how far from the previous ring this ring is)
		v.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices->push_back(v);
	}
}

void MeshTools::linkVertexRings(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, int sides)
{
	int last = vertices->size() - 1;
	for (int i = 0; i < sides; i++)
	{
		if (i < sides - 1)
		{
			//first triangle
			indices->push_back(last - i);
			indices->push_back(last - i - 1);
			indices->push_back(last - i - sides);
			//second triangle
			indices->push_back(last - i - 1);
			indices->push_back(last - i - sides);
			indices->push_back(last - i - 1 - sides);
		}
		else   //we have to connect back to the start
		{
			//first triangle
			indices->push_back(last - i);
			indices->push_back(last);
			indices->push_back(last - i - sides);
			//second triangle
			indices->push_back(last);
			indices->push_back(last - i - sides);
			indices->push_back(last - sides);
		}
	}
}

void MeshTools::linkVertexRingsAtPosition(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, int sides, int last1, int last2)
{
	for (int i = 0; i < sides; i++)
	{
		if (i < sides - 1)
		{
			//first triangle
			indices->push_back(last1 - i);
			indices->push_back(last1 - i - 1);
			indices->push_back(last2 - i);
			//second triangle
			indices->push_back(last1 - i - 1);
			indices->push_back(last2 - i);
			indices->push_back(last2 - i - 1);
		}
		else   //we have to connect back to the start
		{
			//first triangle
			indices->push_back(last1 - i);
			indices->push_back(last1);
			indices->push_back(last2 - i);
			//second triangle
			indices->push_back(last1);
			indices->push_back(last2 - i);
			indices->push_back(last2);
		}
	}
}

void MeshTools::ringToPoint(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 point, int sides)
{
	//1. Create a vertex for the cap
	ColouredVertex cap;
	cap.position = glm::vec4(point.x, point.y, point.z, 1.0f);
	cap.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	cap.texCoords = glm::vec2(0.0f, 1.0f);	//not exactly right
	cap.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices->push_back(cap);
	//2. Link the cap to the ring
	int lastVertex = vertices->size() - 1;
	for (int i = 1; i <= sides; i++)
	{
		if (i < sides)
		{
			indices->push_back(lastVertex);
			indices->push_back(lastVertex - i);
			indices->push_back(lastVertex - i - 1);
		}
		else
		{
			//link back to the start
			indices->push_back(lastVertex);
			indices->push_back(lastVertex - sides);
			indices->push_back(lastVertex - 1);
		}
	}

}

void MeshTools::ringToPointAtPosition(std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 point, int sides, int ringLast)
{
	//1. Create a vertex for the cap
	ColouredVertex cap;
	cap.position = glm::vec4(point.x, point.y, point.z, 1.0f);
	cap.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	cap.texCoords = glm::vec2(0.0f, 1.0f);	//not exactly right
	cap.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices->push_back(cap);
	//2. Link the cap to the ring
	int capVertex = vertices->size() - 1;


	for (int i = 0; i < sides; i++)
	{
		if (i < sides - 1)
		{
			indices->push_back(capVertex);
			indices->push_back(ringLast - i);
			indices->push_back(ringLast - i - 1);
		}
		else
		{
			//link back to the start
			indices->push_back(capVertex);
			indices->push_back(ringLast - i);
			indices->push_back(ringLast);
		}
	}
}

glm::vec3 MeshTools::wobblePoint(glm::vec3 point, float maxDistance)
{
	float rX = (RandomFloat() * 2.0f * maxDistance) - maxDistance;
	float rY = (RandomFloat() * 2.0f * maxDistance) - maxDistance;
	float rZ = (RandomFloat() * 2.0f * maxDistance) - maxDistance;
	return glm::vec3(point.x + rX, point.y + rY, point.z + rZ);
}

glm::vec3 MeshTools::randomPointInUnitSphere()
{
	float x, y, z;
	do
	{
		x = RandomFloat() * 2 - 1;
		y = RandomFloat() * 2 - 1;
		z = RandomFloat() * 2 - 1;
	} while (x*x + y*y + z*z > 1);
	return glm::vec3(x, y, z);
}

glm::vec2 MeshTools::randomPointInUnitCircle()
{
	float x, y;
	do
	{
		x = RandomFloat() * 2 - 1;
		y = RandomFloat() * 2 - 1;
	} while (x*x + y*y > 1);
	return glm::vec2(x, y);
}

void MeshTools::createSphere(glm::vec3 center, float radius, int rings, int sectors, std::vector<ColouredVertex>* vertices, std::vector<unsigned int>* indices)
{
	const float R = 1.0f / (float)(rings - 1);
	const float S = 1.0f / (float)(sectors - 1);
	int r, s;

	int initialVertexCount = vertices->size();
	vertices->reserve(initialVertexCount + (rings * sectors));

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			const float y = sin((glm::pi<float>() / 2.0f) + glm::pi<float>() * r * R);
			const float x = cos(2.0f * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);
			const float z = sin(2.0f * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);
			ColouredVertex v;
			glm::vec3 pos = (glm::vec3(x, y, z) * radius) + center;
			v.position = glm::vec4(pos, 1.0f);
			v.normal = glm::vec3(x, y, z);
			v.texCoords = glm::vec2(s*S, r*R);
			v.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			vertices->push_back(v);
		}
	}

	indices->reserve(indices->size() + (rings * sectors * 6));
	for (r = 0; r < rings - 1; r++)
	{
		for (s = 0; s < sectors - 1; s++)
		{
			indices->push_back(initialVertexCount + (r * sectors + s));
			indices->push_back(initialVertexCount + (r * sectors + (s + 1)));
			indices->push_back(initialVertexCount + ((r + 1) * sectors + (s + 1)));

			indices->push_back(initialVertexCount + (r * sectors + s));
			indices->push_back(initialVertexCount + ((r + 1) * sectors + (s + 1)));
			indices->push_back(initialVertexCount + ((r + 1) * sectors + s));
		}
	}
}