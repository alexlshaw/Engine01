#include "Skybox.h"

Skybox::Skybox(GraphicsResourceManager* resourceManager, glm::vec3 sunPosition)
{
	updateSunPosition(lightPos);
	sunTex = resourceManager->loadTexture("Sun512");
	loadShaders(resourceManager);
	buildMeshes();
}

Skybox::~Skybox()
{
	delete skyboxOuter;
}

void Skybox::loadShaders(GraphicsResourceManager* resourceManager)
{
	skyShader = resourceManager->loadShader("environment/skybox");
	skyShader_horizon = skyShader->getUniformLocation("horizon");
	skyShader_top = skyShader->getUniformLocation("top");
	skyShader_projection = skyShader->getUniformLocation("projectionMatrix");
	skyShader_view = skyShader->getUniformLocation("viewMatrix");
	skyShader_sunPos = skyShader->getUniformLocation("normalizedSunPosition");

	sunShader = resourceManager->loadShader("environment/Sun");
	sunShader_view = sunShader->getUniformLocation("viewMatrix");
	sunShader_projection = sunShader->getUniformLocation("projectionMatrix");
	sunShader_sunPosition = sunShader->getUniformLocation("sunPosition");
	sunShader_tex = sunShader->getUniformLocation("tex");
}

void Skybox::buildMeshes()
{
	float sunSize = 0.1f;
	int skyBoxSides = 32;
	//outer cube
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;

	MeshTools::addVertexRing(&vertices, glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, skyBoxSides);
	MeshTools::addVertexRing(&vertices, glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, skyBoxSides);
	MeshTools::linkVertexRings(&vertices, &indices, skyBoxSides);
	MeshTools::ringToPoint(&vertices, &indices, glm::vec3(0.0f, 10.0f, 0.0f), skyBoxSides);


	skyboxOuter = new Mesh(vertices, indices);
	//sun is just a square
	vertices.clear();
	indices.clear();
	ColouredVertex v1, v2, v3, v4;
	v1.position = glm::vec4(-sunSize, 0.0f, -sunSize, 1.0f);
	v2.position = glm::vec4(-sunSize, 0.0f, sunSize, 1.0f);
	v3.position = glm::vec4(sunSize, 0.0f, sunSize, 1.0f);
	v4.position = glm::vec4(sunSize, 0.0f, -sunSize, 1.0f);
	v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v4.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v1.texCoords = glm::vec2(0.0f, 0.0f);
	v2.texCoords = glm::vec2(0.0f, 1.0f);
	v3.texCoords = glm::vec2(1.0f, 1.0f);
	v4.texCoords = glm::vec2(1.0f, 0.0f);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	sunMesh = new Mesh(vertices, indices);

	//clouds are maybe a smaller cube? Flat plane underneath top of sky?
	//will require further thought
}

void Skybox::draw(Camera* camera)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glm::vec3 horizon = glm::vec3(60.0f, 92.0f, 170.0f) / 255.0f;
	glm::vec3 sky = glm::vec3(27.0f, 64.0f, 140.0f) / 255.0f;

	glm::mat4 projMat = camera->getProjectionMatrix();
	glm::mat4 viewMat = camera->getViewMatrixNoPosition();
	//draw the outer skybox
	skyShader->use();
	skyShader->setUniform(skyShader_projection, projMat);
	skyShader->setUniform(skyShader_view, viewMat);
	skyShader->setUniform(skyShader_horizon, horizon);
	skyShader->setUniform(skyShader_top, sky);
	skyShader->setUniform(skyShader_sunPos, sunPosition);
	skyboxOuter->draw();

	//clear depth buffer
	//glClear(GL_DEPTH_BUFFER_BIT);
	//if daytime, draw day objects
	//1:  the sun
	sunShader->use();
	sunShader->setUniform(sunShader_projection, projMat);
	sunShader->setUniform(sunShader_view, viewMat);
	sunShader->setUniform(sunShader_sunPosition, sunPosition);
	sunShader->setUniform(sunShader_tex, 0);
	glActiveTexture(GL_TEXTURE0);
	sunTex->use();
	sunMesh->draw();

	//2: the clouds

	//if nighttime, draw night objects

	//1: the stars

	//2: the moon

	//finally clear the depth buffer again
	glEnable(GL_DEPTH_TEST);
	//glClear(GL_DEPTH_BUFFER_BIT);
}

void Skybox::updateSunPosition(glm::vec3 newPosition)
{
	sunPosition = glm::normalize(newPosition);
}