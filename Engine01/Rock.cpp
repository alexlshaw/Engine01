#include "Rock.h"

Rock::Rock(RockType* rockType)
{
	this->drawDistanceScale = ROCK_DRAW_SCALE;
	mesh = nullptr;
	materials.push_back(rockType->material);
	buildMeshScatterMethod(rockType);
}

Rock::~Rock()
{
	if (mesh != nullptr)
	{
		delete mesh;
	}
}

void Rock::draw(int renderPass, Transform* transform)
{
	materials[renderPass]->setTransform(transform);
	if (renderPass == 0)
	{
		mesh->draw();
	}
	
}

//scatter spheres and shrink wrap a large sphere around them
void Rock::buildMeshScatterMethod(RockType* rockType)
{
	float shellRadius = rockType->scatterRadius + rockType->pointRadius * 2;
	//float noisyness = 0.5f;
	std::vector<glm::vec3> points;
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;

	//step 1: Generate scattered Points
	points.reserve(rockType->pointCount);
	for (int i = 0; i < rockType->pointCount; i++)
	{
		glm::vec3 point = MeshTools::randomPointInUnitSphere() * rockType->scatterRadius;
		point.y *= rockType->pointYScaling;
		points.push_back(point);
	}
	//add a point in the centre to prevent gaps
	points.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	//step 2: Build outer Sphere mesh
	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
	MeshTools::createSphere(center, shellRadius, 12, 24, &vertices, &indices);

	//step 3: Shrink wrap sphere to scattered points
	for (unsigned int v = 0; v < vertices.size(); v++)
	{
		glm::vec3 hit;
		glm::vec3 hitNormal;
		glm::vec3 rayStart = glm::vec3(vertices[v].position);
		glm::vec3 rayDir = glm::normalize(center - rayStart);
		glm::vec3 outermostHitPoint;
		glm::vec3 outermostNormal;
		float outermostLength = shellRadius;// - minimumRadius;	//the distance in from the shell of the hit (we try minimise this)
		for (unsigned int p = 0; p < points.size(); p++)
		{
			if (Collision::raySphereCollision(rayStart, rayDir, points[p], rockType->pointRadius, hit, hitNormal))
			{
				//check to see if it is the outermost hit so far for this vertex
				float inwardDistance = shellRadius - glm::length(hit - center);
				if (inwardDistance < outermostLength && inwardDistance > 0.0f)
				{
					outermostHitPoint = hit;
					outermostNormal = hitNormal;
					outermostLength = inwardDistance;
				}
			}
		}
		//bring the vertex inwards to the detected point
		//float noise = RandomFloat() - noisyness;		//noise looks decent, but multiple vertices in the same location
														//can end up in different positions, causing splits
		glm::vec3 noiseOffset = glm::vec3(0.0f, 0.0f, 0.0f);// (noise * outermostNormal);
		vertices[v].position = glm::vec4(outermostHitPoint + noiseOffset, 1.0f);
		vertices[v].normal = outermostNormal;
	}

	//step 4: push mesh
	mesh = new Mesh(vertices, indices);
}