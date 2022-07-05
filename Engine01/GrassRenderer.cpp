#include "GrassRenderer.h"

GrassRenderer::GrassRenderer(GraphicsResourceManager* resourceManager)
{
	DEBUG_PRINT("Building Grass Renderer\n");
	loadShader(resourceManager);
	generateMesh();
	grasses = resourceManager->loadTexture("GrassBillboard2");	//use a grass texture for testing
	DEBUG_PRINT("Finished building Grass Renderer\n");
}

GrassRenderer::~GrassRenderer()
{
	glDeleteBuffers(1, &rVBO);
	glDeleteBuffers(1, &rIBO);
}

void GrassRenderer::loadShader(GraphicsResourceManager* resourceManager)
{
	grass = resourceManager->loadShader("environment/grass");
	grass_modelView = grass->getUniformLocation("modelview_matrix");
	grass_projection = grass->getUniformLocation("projection_matrix");
	grass_transform = grass->getUniformLocation("transform");
	grass_heightMapTexture = grass->getUniformLocation("heightMapTexture");
	grass_splatMapTexture = grass->getUniformLocation("splatMapTexture");
	grass_textures = grass->getUniformLocation("textures");
}

void GrassRenderer::generateMesh()
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//start with a regular grid of tufts spaced close together
	for (float x = -49.0f; x < 50.0f; x += 1.0f)
	{
		for (float z = -49.0f; z < 50.0f; z += 1.0f)
		{
			float yFactor = (x * x + z * z) / 2500.0f;
			addTuft(x, yFactor, z, &vertices, &indices);
		}
	}
	pushMesh(vertices, indices);
}

void GrassRenderer::pushMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	DEBUG_PRINT_GL_ERRORS();
	totalVertices = vertices.size();
	glGenVertexArrays(1, &rVAO);
	glGenBuffers(1, &rVBO);
	glBindVertexArray(rVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rVBO);
	//push vertices into the array here, and then set the vertex attributes for the shader's use
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * totalVertices, &vertices[0], GL_STATIC_DRAW);
	setVertexAttribs();
	//now set up the index buffer
	totalIndices = indices.size();
	glGenBuffers(1, &rIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndices * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR)
	{
		DEBUG_PRINT("Error loading grass renderer vertex data.\n");
	}
}

void GrassRenderer::addTuft(float x, float y, float z, std::vector<Vertex>* vertices, std::vector<unsigned int>* indices)
{
	float tuftSize = 0.8f;// 0.3535f;
	float tuftHeight = 1.5f;
	Vertex v1, v2, v3, v4;
	/*  v3----v4
		|  /  |
		v1----v2	*/
	unsigned int firstIndex = vertices->size();
	//first quad
	v1.position = glm::vec3(x - tuftSize, -y, z - tuftSize);
	v1.texCoords = glm::vec2(0.0f, 0.0f);
	v2.position = glm::vec3(x + tuftSize, -y, z + tuftSize);
	v2.texCoords = glm::vec2(0.5f, 0.0f);
	v3.position = glm::vec3(x - tuftSize, tuftHeight - y, z - tuftSize);
	v3.texCoords = glm::vec2(0.0f, 0.99f);
	v4.position = glm::vec3(x + tuftSize, tuftHeight - y, z + tuftSize);
	v4.texCoords = glm::vec2(0.5f, 0.99f);
	vertices->push_back(v1);
	vertices->push_back(v2);
	vertices->push_back(v3);
	vertices->push_back(v4);
	//indices
	indices->push_back(firstIndex);		//v1
	indices->push_back(firstIndex + 2);	//v3
	indices->push_back(firstIndex + 3);	//v4
	indices->push_back(firstIndex);		//v1
	indices->push_back(firstIndex + 3);	//v4
	indices->push_back(firstIndex + 1);	//v2
	//second quad
	v1.position = glm::vec3(x - tuftSize, -y, z + tuftSize);
	v1.texCoords = glm::vec2(0.0f, 0.0f);
	v2.position = glm::vec3(x + tuftSize, -y, z - tuftSize);
	v2.texCoords = glm::vec2(0.5f, 0.0f);
	v3.position = glm::vec3(x - tuftSize, tuftHeight - y, z + tuftSize);
	v3.texCoords = glm::vec2(0.0f, 0.99f);
	v4.position = glm::vec3(x + tuftSize, tuftHeight - y, z - tuftSize);
	v4.texCoords = glm::vec2(0.5f, 0.99f);
	vertices->push_back(v1);
	vertices->push_back(v2);
	vertices->push_back(v3);
	vertices->push_back(v4);
	//indices
	firstIndex += 4;	//move to the starting index of the second quad
	indices->push_back(firstIndex);		//v1
	indices->push_back(firstIndex + 2);	//v3
	indices->push_back(firstIndex + 3);	//v4
	indices->push_back(firstIndex);		//v1
	indices->push_back(firstIndex + 3);	//v4
	indices->push_back(firstIndex + 1);	//v2
}

void GrassRenderer::updateTransform(Camera* camera)
{
	transform.setPosition(glm::vec3(floor(camera->position.x), 0.0f, floor(camera->position.z)));
}

void GrassRenderer::draw(Camera* camera, World* world)
{
	glEnable(GL_BLEND);
	//get matrices
	updateTransform(camera);
	glm::mat4 viewMat = camera->getViewMatrix();
	glm::mat4 projectionMat = camera->getProjectionMatrix();
	glm::mat4 modelMat = transform.getMatrix();

	//set up heightmap stuff
	GLint heightMaps[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	GLint splatMaps[9] = { 9, 10, 11, 12, 13, 14, 15, 16, 17 };
	std::vector<int> grid = world->grid.iterate();
	for (int i = 0; i < 9; i++)
	{
		Node* node = &(world->nodes[grid[i]]);
		if (node->state != READY)
		{
			//we require that the heightMap and splatmap are valid textures to pass to the shader (the node must have a state of READY)
			//thus if the node is not ready, we replace it with the center node, which World GUARANTEES to be valid (READY)
			node = &(world->nodes[world->grid.Center]);
		}
		//set the height map texture
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, node->heightMapTexture);
		glActiveTexture(GL_TEXTURE9 + i);
		glBindTexture(GL_TEXTURE_2D, node->splatMapTexture);
	}
	//setup drawing textures
	glActiveTexture(GL_TEXTURE18);
	grasses->use();

	//setup shader
	grass->use();
	grass->setUniform(grass_modelView, viewMat);
	grass->setUniform(grass_projection, projectionMat);
	grass->setUniform(grass_transform, modelMat);
	glUniform1iv(grass_heightMapTexture, 9, heightMaps);
	glUniform1iv(grass_splatMapTexture, 9, splatMaps);
	grass->setUniform(grass_textures, 18);
	
	//then bind the mesh and draw it
	glBindVertexArray(rVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rIBO);
	glDrawElements(GL_TRIANGLES, totalIndices, GL_UNSIGNED_INT, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}