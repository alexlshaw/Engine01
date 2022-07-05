#include "OceanRenderer.h"

OceanRenderer::OceanRenderer()
{
	DEBUG_PRINT("Building ocean renderer\n");
	loadShader();
	generateMesh();
	//waterTex.load("Data/Water1024.tga");
	DEBUG_PRINT("Finished building ocean renderer\n");
}

void OceanRenderer::loadShader()
{
	ocean = Shader();
	ocean.compileShaderFromFile("./Data/Shaders/environment/Ocean.vert", VERTEX);
	ocean.compileShaderFromFile("./Data/Shaders/environment/Ocean.frag", FRAGMENT);
	ocean.linkAndValidate();
	modelMatrix = ocean.getUniformLocation("modelMatrix");
	viewMatrix = ocean.getUniformLocation("viewMatrix");
	projectionMatrix = ocean.getUniformLocation("projectionMatrix");
	//texUniform = ocean.getUniformLocation("waterTex");
}

void OceanRenderer::pushMesh(std::vector<SimpleVertex> vertices, std::vector<unsigned int> indices)
{
	DEBUG_PRINT_GL_ERRORS();
	totalVertices = vertices.size();
	glGenVertexArrays(1, &rVAO);
	glGenBuffers(1, &rVBO);
	glBindVertexArray(rVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rVBO);
	//push vertices into the array here, and then set the vertex attributes for the shader's use
	glBufferData(GL_ARRAY_BUFFER, sizeof(SimpleVertex) * totalVertices, &vertices[0], GL_STATIC_DRAW);
	setSimpleVertexAttribs();
	//now set up the index buffer
	totalIndices = indices.size();
	glGenBuffers(1, &rIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndices * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR)
	{
		DEBUG_PRINT("Error loading terrain renderer vertex data.\n");
	}
}

void OceanRenderer::generateMesh()
{
	//I've been having some OpenGL crashes with very large meshes. I believe the problem is when the mesh grows big enough to pull heights from 2 nodes away, 
	//we get an array index out of bounds situation when selecting which texture(node) to pull heightmap data for
	int segmentQuadCount = 64;
	int totalSegments = 4;
	std::vector<SimpleVertex> meshVertices;
	std::vector<unsigned int> indices;
	int currentQuadWidth = 1;
	int innerRadius = 0;
	for (int i = 0; i < totalSegments; i++)
	{
		int outerRadius = innerRadius + (currentQuadWidth * segmentQuadCount);
		buildMeshSegmentSided(&meshVertices, &indices, innerRadius, outerRadius, currentQuadWidth);
		currentQuadWidth *= 2;
		innerRadius = outerRadius;
	}
	gridSnap = currentQuadWidth / 2;
	meshRadius = innerRadius;
	DEBUG_PRINT_GL_ERRORS();
	pushMesh(meshVertices, indices);
}

void OceanRenderer::buildMeshSegment(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int outerRadius, int quadSize)
{
	if (innerRadius == 0)
	{
		DEBUG_PRINT("Building innermost segment of terrain renderer mesh\n");
		//if the inner radius is 0, we obviously don't need to build a skirt
		//at inner radius 0, we are simply building a 2xquadSize by 2*quadSize grid of quadsize tiles, centered on the origin
		buildRectangularSegment(vertices, indices, -outerRadius, -outerRadius, 2 * outerRadius, 2 * outerRadius, quadSize);
	}
	else
	{
		buildMeshSegmentSkirt(vertices, indices, innerRadius, quadSize);
		int outsideOfSkirt = innerRadius + quadSize;
		//now that the skirt is finished, we fill out the rest of the space with quadsize tiles
		//since we are skipping the interior, we can do this with 4 rectangular segments
		buildRectangularSegment(vertices, indices, -outerRadius, -outerRadius, 2 * outerRadius, outerRadius - outsideOfSkirt, quadSize);
		buildRectangularSegment(vertices, indices, -outerRadius, outsideOfSkirt, 2 * outerRadius, outerRadius - outsideOfSkirt, quadSize);
		buildRectangularSegment(vertices, indices, -outerRadius, -outsideOfSkirt, outerRadius - outsideOfSkirt, 2 * outsideOfSkirt, quadSize);
		buildRectangularSegment(vertices, indices, outsideOfSkirt, -outsideOfSkirt, outerRadius - outsideOfSkirt, 2 * outsideOfSkirt, quadSize);
	}
}

void OceanRenderer::buildMeshSegmentSided(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int outerRadius, int quadSize)
{
	//if this is the innermost segment, just build a rectangular block
	if (innerRadius == 0)
	{
		buildRectangularSegment(vertices, indices, -outerRadius, 0, 2 * outerRadius, 1 * outerRadius, quadSize);
	}
	//give it a skirt
	buildMeshSegmentSkirtSided(vertices, indices, innerRadius, quadSize);
	int outsideOfSkirt = innerRadius + quadSize;
	buildRectangularSegment(vertices, indices, -outerRadius, -outerRadius, 2 * outerRadius, outerRadius - outsideOfSkirt, quadSize);
	buildRectangularSegment(vertices, indices, -outerRadius, -outsideOfSkirt, outerRadius - outsideOfSkirt, outsideOfSkirt, quadSize);
	buildRectangularSegment(vertices, indices, outsideOfSkirt, -outsideOfSkirt, outerRadius - outsideOfSkirt, outsideOfSkirt, quadSize);
}

void OceanRenderer::buildRectangularSegment(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int startingX, int startingZ, int xSize, int zSize, int quadSize)
{
	unsigned int originalStart = vertices->size();
	//add the vertices (very simple, just lay down a grid)
	for (int x = startingX; x <= startingX + xSize; x += quadSize)
	{
		float fx = (float)x;
		for (int z = startingZ; z <= startingZ + zSize; z += quadSize)
		{
			float fz = (float)z;
			vertices->push_back(SimpleVertex(glm::vec3(fx, 0.0f, fz)));
		}
	}
	//add the indices (diagram below each number represents a vertex. numbers given in index specifications assume overall mesh matching that diagram -> for other sizes the numbers will change
	//0-3-6
	//1-4-7
	//2-5-8
	int xCount = xSize / quadSize;
	int zCount = zSize / quadSize;
	unsigned int endingIndex = originalStart - 1 + ((xCount + 1) * (zCount + 1));
	unsigned int colOffset1 = zCount + 1;
	int colOffset2 = zCount + 2;
	for (unsigned int currentIndex = originalStart + colOffset2; currentIndex <= endingIndex; currentIndex++)
	{
		if ((currentIndex - originalStart) % colOffset1 != 0)	//skip the vertex at the start of each column
		{
			indices->push_back(currentIndex);		//4/5/7/8
			indices->push_back(currentIndex - 1);	//3
			indices->push_back(currentIndex - colOffset1);	//1

			indices->push_back(currentIndex - colOffset1);	//1
			indices->push_back(currentIndex - 1);	//3
			indices->push_back(currentIndex - colOffset2);	//0
		}
	}
}

void OceanRenderer::buildMeshSegmentSkirt(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int quadSize)		//TODO: create sided version of this method
{
	//if the inner radius is not 0, we assume that the quadSize of the previous segment was 1 lower than the current quadSize
	//so we first build a skirt around the inner radius grid that skirts between the previous quadsize and the current one
	float fq = (float)quadSize;
	float fiq = fq / 2.0f;		//float version of inner quad size
	int outsideOfSkirt = innerRadius + quadSize;
	//Build the vertices as follows:
	//	  z2|-------------|-------------|-------------|-------------|
	//		| \         / | \         / | \         / | \         / |
	//		|  \   2   /  |  \   2   /  |  \   2   /  |  \   2   /  |
	//		|  1 \   / 3  |  1 \   / 3  |  1 \   / 3  |  1 \   / 3  |
	//		|     \ /     |     \ /     |     \ /     |     \ /     |
	//	  z1|-------------|-------------|-------------|-------------|
	//	  x:-ir													   x:+ir

	//build the skirting sections, sans corners
	for (int x = -innerRadius; x < innerRadius; x += quadSize)
	{
		float z1 = (float)(-innerRadius);
		float z2 = (float)(-outsideOfSkirt);
		float fx = (float)x;
		//build the -z section
		addTriangle(vertices, indices, glm::vec3(fx, 0.0f, z1), glm::vec3(fx, 0.0f, z2), glm::vec3(fx + fiq, 0.0f, z1), fq);	//triangle #1
		addTriangle(vertices, indices, glm::vec3(fx, SEA_LEVEL, z2), glm::vec3(fx + fq, SEA_LEVEL, z2), glm::vec3(fx + fiq, 0.0f, z1), fq);	//triangle 2
		addTriangle(vertices, indices, glm::vec3(fx + fq, 0.0f, z2), glm::vec3(fx + fq, 0.0f, z1), glm::vec3(fx + fiq, 0.0f, z1), fq);	//triangle 3
		//build the +z section
		z1 *= -1.0f;
		z2 *= -1.0f;
		addTriangle(vertices, indices, glm::vec3(fx, 0.0f, z1), glm::vec3(fx, 0.0f, z2), glm::vec3(fx + fiq, 0.0f, z1), fq);	//triangle #1
		addTriangle(vertices, indices, glm::vec3(fx, 0.0f, z2), glm::vec3(fx + fq, 0.0f, z2), glm::vec3(fx + fiq, 0.0f, z1), fq);	//triangle 2
		addTriangle(vertices, indices, glm::vec3(fx + fq, 0.0f, z2), glm::vec3(fx + fq, 0.0f, z1), glm::vec3(fx + fiq, 0.0f, z1), fq);	//triangle 3

	}
	for (int z = -innerRadius; z < innerRadius; z += quadSize)
	{
		float x1 = (float)(-innerRadius);
		float x2 = (float)(-outsideOfSkirt);
		float fz = (float)z;
		//build the -x section
		addTriangle(vertices, indices, glm::vec3(x1, 0.0f, fz), glm::vec3(x2, 0.0f, fz), glm::vec3(x1, 0.0f, fz + fiq), fq);
		addTriangle(vertices, indices, glm::vec3(x2, 0.0f, fz), glm::vec3(x2, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fiq), fq);
		addTriangle(vertices, indices, glm::vec3(x2, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fiq), fq);
		//build the +x section
		x1 *= -1.0f;
		x2 *= -1.0f;
		addTriangle(vertices, indices, glm::vec3(x1, 0.0f, fz), glm::vec3(x2, 0.0f, fz), glm::vec3(x1, 0.0f, fz + fiq), fq);
		addTriangle(vertices, indices, glm::vec3(x2, 0.0f, fz), glm::vec3(x2, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fiq), fq);
		addTriangle(vertices, indices, glm::vec3(x2, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fiq), fq);
	}
	//build the corners (1x1 rectangular segments)
	buildRectangularSegment(vertices, indices, -outsideOfSkirt, -outsideOfSkirt, quadSize, quadSize, quadSize);
	buildRectangularSegment(vertices, indices, innerRadius, -outsideOfSkirt, quadSize, quadSize, quadSize);
	buildRectangularSegment(vertices, indices, innerRadius, innerRadius, quadSize, quadSize, quadSize);
	buildRectangularSegment(vertices, indices, -outsideOfSkirt, innerRadius, quadSize, quadSize, quadSize);

}

void OceanRenderer::buildMeshSegmentSkirtSided(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int quadSize)		//TODO: create sided version of this method
{
	//if the inner radius is not 0, we assume that the quadSize of the previous segment was 1 lower than the current quadSize
	//so we first build a skirt around the inner radius grid that skirts between the previous quadsize and the current one
	float fq = (float)quadSize;
	float fiq = fq / 2.0f;		//float version of inner quad size
	int outsideOfSkirt = innerRadius + quadSize;
	//Build the vertices as follows:
	//	  z2|-------------|-------------|-------------|-------------|
	//		| \         / | \         / | \         / | \         / |
	//		|  \   2   /  |  \   2   /  |  \   2   /  |  \   2   /  |
	//		|  1 \   / 3  |  1 \   / 3  |  1 \   / 3  |  1 \   / 3  |
	//		|     \ /     |     \ /     |     \ /     |     \ /     |
	//	  z1|-------------|-------------|-------------|-------------|
	//	  x:-ir													   x:+ir

	//build the skirting sections, sans corners
	for (int x = -innerRadius; x < innerRadius; x += quadSize)
	{
		float z1 = (float)(-innerRadius);
		float z2 = (float)(-outsideOfSkirt);
		float fx = (float)x;
		//build the -z section
		addTriangle(vertices, indices, glm::vec3(fx, 0.0f, z1), glm::vec3(fx, 0.0f, z2), glm::vec3(fx + fiq, 0.0f, z1), fq);	//triangle #1
		addTriangle(vertices, indices, glm::vec3(fx, 0.0f, z2), glm::vec3(fx + fq, 0.0f, z2), glm::vec3(fx + fiq, 0.0f, z1), fq);	//triangle 2
		addTriangle(vertices, indices, glm::vec3(fx + fq, 0.0f, z2), glm::vec3(fx + fq, 0.0f, z1), glm::vec3(fx + fiq, 0.0f, z1), fq);	//triangle 3
	}
	for (int z = -innerRadius; z < 0; z += quadSize)
	{
		float x1 = (float)(-innerRadius);
		float x2 = (float)(-outsideOfSkirt);
		float fz = (float)z;
		//build the -x section
		addTriangle(vertices, indices, glm::vec3(x1, 0.0f, fz), glm::vec3(x2, 0.0f, fz), glm::vec3(x1, 0.0f, fz + fiq), fq);
		addTriangle(vertices, indices, glm::vec3(x2, 0.0f, fz), glm::vec3(x2, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fiq), fq);
		addTriangle(vertices, indices, glm::vec3(x2, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fiq), fq);
		//build the +x section
		x1 *= -1.0f;
		x2 *= -1.0f;
		addTriangle(vertices, indices, glm::vec3(x1, 0.0f, fz), glm::vec3(x2, 0.0f, fz), glm::vec3(x1, 0.0f, fz + fiq), fq);
		addTriangle(vertices, indices, glm::vec3(x2, 0.0f, fz), glm::vec3(x2, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fiq), fq);
		addTriangle(vertices, indices, glm::vec3(x2, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fq), glm::vec3(x1, 0.0f, fz + fiq), fq);
	}
	//build the corners (1x1 rectangular segments)
	buildRectangularSegment(vertices, indices, -outsideOfSkirt, -outsideOfSkirt, quadSize, quadSize, quadSize);
	buildRectangularSegment(vertices, indices, innerRadius, -outsideOfSkirt, quadSize, quadSize, quadSize);
}

void OceanRenderer::addTriangle(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float quadSize)
{
	unsigned int startingIndex = vertices->size();
	vertices->push_back(SimpleVertex(v1));
	vertices->push_back(SimpleVertex(v2));
	vertices->push_back(SimpleVertex(v3));
	indices->push_back(startingIndex);
	indices->push_back(++startingIndex);
	indices->push_back(++startingIndex);
}

void OceanRenderer::updateTransform(Camera* camera)
{
	//snaps the position to a widely spaced grid to avoid jitter
	int newX = (int)camera->position.x - (int)camera->position.x % gridSnap;
	int newZ = (int)camera->position.z - (int)camera->position.z % gridSnap;
	transform.setPosition(glm::vec3((float)newX, SEA_LEVEL, (float)newZ));

	float angle = 270.0f;
	if (camera->horizontalAngle > 45.0f && camera->horizontalAngle <= 135.0f)			//hAngle around 90
	{
		angle = 180.0f;
	}
	else if (camera->horizontalAngle > 135.0f && camera->horizontalAngle <= 225.0f)		//hAngle around 180
	{
		angle = 90.0f;
	}
	else if (camera->horizontalAngle > 225.0f && camera->horizontalAngle <= 315.0f)		//hAngle around 270
	{
		angle = 0.0f;
	}

	transform.setRotation(glm::vec4(0.0f, 1.0f, 0.0f, angle));
}

void OceanRenderer::draw(Camera* camera)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//activate texture
	//glActiveTexture(GL_TEXTURE0);
	//waterTex.use();
	//get matrices
	updateTransform(camera);
	glm::mat4 vMatrix = camera->getViewMatrix();
	glm::mat4 pMatrix = camera->getProjectionMatrix();
	glm::mat4 mMatrix = transform.getMatrix();
	//set shader uniforms
	ocean.use();
	ocean.setUniform(viewMatrix, vMatrix);
	ocean.setUniform(projectionMatrix, pMatrix);
	ocean.setUniform(modelMatrix, mMatrix);
	//ocean.setUniform(texUniform, 0);
	//bind the mesh and draw it
	glBindVertexArray(rVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rIBO);
	glDrawElements(GL_TRIANGLES, totalIndices, GL_UNSIGNED_INT, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

OceanRenderer::~OceanRenderer()
{
	glDeleteBuffers(1, &rVBO);
	glDeleteBuffers(1, &rIBO);
}