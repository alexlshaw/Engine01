#include "TerrainRenderer.h"

TerrainRenderer::TerrainRenderer(GraphicsResourceManager* resourceManager)
{
	DEBUG_PRINT("Building terrain renderer\n");
	this->resourceManager = resourceManager;
	loadShader();
	generateMesh();
	dirtLayer = resourceManager->loadTexture("Dirt1024");
	grassLayer = resourceManager->loadTexture("Grass1024");
	rockLayer = resourceManager->loadTexture("Rock1024");
	sandLayer = resourceManager->loadTexture("Sand1024");
	snowLayer = resourceManager->loadTexture("Snow1024");
	DEBUG_PRINT("Finished building terrain renderer.\n");
}

void TerrainRenderer::loadShader()
{
	terrain2 = Shader();
	terrain2.compileShaderFromFile("./Data/Shaders/environment/Terrain2.vert", VERTEX);
	terrain2.compileShaderFromFile("./Data/Shaders/environment/Terrain2.frag", FRAGMENT);
	terrain2.linkAndValidate();
	terrain2_modelView = terrain2.getUniformLocation("modelview_matrix");
	terrain2_projection = terrain2.getUniformLocation("projection_matrix");
	terrain2_transform = terrain2.getUniformLocation("transform");
	terrain2_heightmapTexture = terrain2.getUniformLocation("heightMapTexture");
	terrain2_splatMaps = terrain2.getUniformLocation("splatMaps");
	//Texturing stuff
	terrain2_rockTex = terrain2.getUniformLocation("rockTex");
	terrain2_sandTex = terrain2.getUniformLocation("sandTex");
	terrain2_dirtTex = terrain2.getUniformLocation("dirtTex");
	terrain2_grassTex = terrain2.getUniformLocation("grassTex");
	terrain2_snowTex = terrain2.getUniformLocation("snowTex");
	//lighting stuff
	terrain2_lightIntensity = terrain2.getUniformLocation("lightIntensity");
	terrain2_ambient = terrain2.getUniformLocation("ambient");
	terrain2_diffuse = terrain2.getUniformLocation("diffuse");
	terrain2_lightPos = terrain2.getUniformLocation("lightPos");
}

void TerrainRenderer::pushMesh(std::vector<SimpleVertex> vertices, std::vector<unsigned int> indices)
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

void TerrainRenderer::generateMesh()
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
		buildMeshSegment(&meshVertices, &indices, innerRadius, outerRadius, currentQuadWidth);
		currentQuadWidth *= 2;
		innerRadius = outerRadius;
	}
	gridSnap = currentQuadWidth / 2;
	meshRadius = innerRadius;
	DEBUG_PRINT_GL_ERRORS();
	pushMesh(meshVertices, indices);
}

void TerrainRenderer::buildMeshSegment(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int outerRadius, int quadSize)
{
	//start with a quick little debug mode sanity check to make sure that the method is being called correctly
	#ifdef _DEBUG
	if ((outerRadius - innerRadius) % quadSize != 0)
	{
		DEBUG_PRINT("Attempting to build a terrain renderer mesh segment with invalid values\n");
	}
	#endif

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

void TerrainRenderer::buildMeshSegmentSided(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int outerRadius, int quadSize)
{
	//this method cannot build the innermost segment
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

void TerrainRenderer::buildRectangularSegment(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int startingX, int startingZ, int xSize, int zSize, int quadSize)
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

void TerrainRenderer::buildMeshSegmentSkirt(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int quadSize)		//TODO: create sided version of this method
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

void TerrainRenderer::buildMeshSegmentSkirtSided(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, int innerRadius, int quadSize)		//TODO: create sided version of this method
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

void TerrainRenderer::addTriangle(std::vector<SimpleVertex>* vertices, std::vector<unsigned int>* indices, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float quadSize)
{
	unsigned int startingIndex = vertices->size();
	vertices->push_back(SimpleVertex(v1));
	vertices->push_back(SimpleVertex(v2));
	vertices->push_back(SimpleVertex(v3));
	indices->push_back(startingIndex);
	indices->push_back(++startingIndex);
	indices->push_back(++startingIndex);
}

void TerrainRenderer::updateTransform(Camera* camera)
{
	//snaps the position to a widely spaced grid to avoid jitter
	int newX = (int)camera->position.x - (int)camera->position.x % gridSnap;
	int newZ = (int)camera->position.z - (int)camera->position.z % gridSnap;
	transform.setPosition(glm::vec3((float)newX, 0.0f, (float)newZ));

	//float angle = 270.0f;
	//if (camera->horizontalAngle > 45.0f && camera->horizontalAngle <= 135.0f)			//hAngle around 90
	//{
	//	angle = 180.0f;
	//}
	//else if (camera->horizontalAngle > 135.0f && camera->horizontalAngle <= 225.0f)		//hAngle around 180
	//{
	//	angle = 90.0f;
	//}
	//else if (camera->horizontalAngle > 225.0f && camera->horizontalAngle <= 315.0f)		//hAngle around 270
	//{
	//	angle = 0.0f;
	//}

	//transform.setRotation(glm::vec4(0.0f, 1.0f, 0.0f, angle));
}

void TerrainRenderer::draw(Camera* camera, World* world)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//start by building the texture arrays to be passed to the shader, and binding the appropriate texture units
	GLint heightMapTextures[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	GLint splatMaps[9] = { 9, 10, 11, 12, 13, 14, 15, 16, 17 };
	std::vector<int> grid = world->grid.iterate();
	for (int i = 0; i < 9; i++)
	{
		Node* node = &(world->nodes[grid[i]]);
		if (node->state != READY)
		{
			//we require that the heightMap and splatMap are valid textures to pass to the shader (the node must have a state of READY)
			//thus if the node is not ready, we replace it with the center node, which World GUARANTEES to be valid (READY)
			node = &(world->nodes[world->grid.Center]);
		}
		//set the height map texture
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, node->heightMapTexture);
		glActiveTexture(GL_TEXTURE9 + i);
		glBindTexture(GL_TEXTURE_2D, node->splatMapTexture);
	}

	//bind the layer textures
	glActiveTexture(GL_TEXTURE18);
	rockLayer->use();
	glActiveTexture(GL_TEXTURE19);
	sandLayer->use();
	glActiveTexture(GL_TEXTURE20);
	dirtLayer->use();
	glActiveTexture(GL_TEXTURE21);
	grassLayer->use();
	glActiveTexture(GL_TEXTURE22);
	snowLayer->use();

	//now that we've handled the textures, set the transform and get the matrices
	updateTransform(camera);
	glm::mat4 mvMatrix = camera->getViewMatrix();
	glm::mat4 pMatrix = camera->getProjectionMatrix();

	//Pass everything to the shader
	glm::mat4 tr = transform.getMatrix();
	terrain2.use();
	terrain2.setUniform(terrain2_modelView, mvMatrix);		//TODO: may want to combine matrices to save redundant computations in shader
	terrain2.setUniform(terrain2_projection, pMatrix);
	terrain2.setUniform(terrain2_transform, tr);
	glUniform1iv(terrain2_heightmapTexture, 9, heightMapTextures);
	glUniform1iv(terrain2_splatMaps, 9, splatMaps);
	terrain2.setUniform(terrain2_rockTex, 18);				//TODO: do uniforms that are not changing need to be set every time?
	terrain2.setUniform(terrain2_sandTex, 19);
	terrain2.setUniform(terrain2_dirtTex, 20);
	terrain2.setUniform(terrain2_grassTex, 21);
	terrain2.setUniform(terrain2_snowTex, 22);
	terrain2.setUniform(terrain2_lightPos, glm::normalize(lightPos));
	terrain2.setUniform(terrain2_lightIntensity, vec3(0.8f, 0.8f, 0.8f));
	terrain2.setUniform(terrain2_ambient, Ka);
	terrain2.setUniform(terrain2_diffuse, Kd);

	//then bind the mesh and draw it
	glBindVertexArray(rVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rIBO);
	glDrawElements(GL_TRIANGLES, totalIndices, GL_UNSIGNED_INT, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	////since the rest of the code doesn't currently set the active texture unit, better put this in here
	//glActiveTexture(GL_TEXTURE0);
}

TerrainRenderer::~TerrainRenderer()
{
	glDeleteBuffers(1, &rVBO);
	glDeleteBuffers(1, &rIBO);
}