#include "Tree.h"

Tree::Tree(TreeSpecies* species)
{
	this->drawDistanceScale = TREE_DRAW_SCALE;
	this->species = species;
	mesh = nullptr;
	leavesMesh = nullptr;
	materials.push_back(species->barkMaterial);
	materials.push_back(species->leavesMaterial);
	buildTreeSpaceColonizationMethod();
}

Tree::~Tree()
{
	if (mesh != nullptr)
	{
		delete mesh;
		mesh = nullptr;
	}
	if (leavesMesh != nullptr)
	{
		delete leavesMesh;
		leavesMesh = nullptr;
	}
}

void Tree::draw(int renderPass, Transform* transform)
{
	materials[renderPass]->setTransform(transform);
	if (renderPass == 0)
	{
		//draw the trunk and branches
		mesh->draw();
	}
	else
	{
		//do the leaves
		leavesMesh->draw();
	}
}

void Tree::buildTreeSpaceColonizationMethod()				//based on: Modeling Trees with a Space Colonization Algorithm, Runions et al
{
	//create set of attraction points
	std::forward_list<glm::vec3> attractionPoints;
	generateAttractionPoints(&attractionPoints);

	//create initial nodes for tree
	std::deque<TreeNode*> nodes;
	TreeNode* base = new TreeNode;
	base->parent = nullptr;
	base->location = glm::vec3(0.0f, 0.0f, 0.0f);
	TreeNode* secondNode = new TreeNode;
	secondNode->parent = base;
	secondNode->location = glm::vec3(0.01f, species->heightToFirstBranches, 0.0f);
	base->children.push_back(secondNode);
	nodes.push_back(base);
	nodes.push_back(secondNode);

	//iteratively build the skeleton
	int iterations = 0;
	while (iterations < species->maxIterations)		//should add a test for there being any attraction points remaining, but that might be O(n) time for a forward list so not worth
	{
		//for each attraction point:
		for (auto& point : attractionPoints)
		{
			//determine the closest node within its radius of influence	(di)
			TreeNode* closest;
			float closestDistance = 9999999.0f;	//arbitrarily high value, I'm assuming that we'll never have to deal with trees of this size
			for (auto& node : nodes)
			{
				float currentDistance = glm::length(point - node->location);
				if (currentDistance < closestDistance)
				{
					closest = node;
					closestDistance = currentDistance;
				}
			}
			//calculate the normalized vector to that node and add to the influence vectors of that node
			closest->influenceVectors.push_back(glm::normalize(point - closest->location));
		}
		//for each node v selected as a closest node:
		std::vector<TreeNode*> temp;
		temp.reserve(100);
		for (auto& v : nodes)
		{
			if (v->influenceVectors.size() > 0)
			{
				//sum the (pre-normalized) vectors to the associated attraction points to get the sum vector (sv)
				glm::vec3 sumVector = glm::vec3(0.0f, 0.0f, 0.0f);
				for (auto& vec : v->influenceVectors)
				{
					sumVector = sumVector + vec;
				}
				//normalize sv
				sumVector = glm::normalize(sumVector);
				//create a new node v' at v + Dsv
				TreeNode* vPrime = new TreeNode;
				vPrime->location = v->location + (sumVector * species->newNodeDistance);
				//attach v' as a child of v
				vPrime->parent = v;
				v->children.push_back(vPrime);
				temp.push_back(vPrime);
				//reset the influence vector list
				v->influenceVectors.clear();
			}
		}
		//move the newly created nodes into the main collection
		for (auto& nn : temp)
		{
			nodes.push_back(nn);
		}
		//for each attraction point, determine if there are any nodes within the killDistance. If so, remove the attraction point
		float kd = species->killDistance;
		attractionPoints.remove_if([&nodes, kd](glm::vec3 p)
		{
			for (auto& node : nodes)
			{
				if (glm::length(p - node->location) < kd)
				{
					return true;
				}
			}
			return false;
		});

		iterations++;
	}
	//at this point we could decimate skeleton nodes to simplify geometry, but looking at the tree we're getting, it's probably unnecessary

	//create mesh based on skeleton (more complex approach using vertex rings)
	TreeNode* root = nodes[0];
	recursivelyCalculateWidths(root);
	MeshTools::addVertexRing(&vertices, root->location, glm::vec3(0.0f, -1.0f, 0.0f), root->radius, TREE_SIDES);	//the root's p-vector is straight down
	root->indexOfLastVertex = vertices.size() - 1;
	for (auto& child : root->children)
	{
		addTreeNodeToMeshRecursive(child);
	}
	//push the mesh to the GPU
	mesh = new Mesh(vertices, indices);
	leavesMesh = new Mesh(leavesVertices, leavesIndices);

	//after creating the mesh, delete the nodes to free up memory
	for (auto& node : nodes)
	{
		delete node;
	}
}

void Tree::generateAttractionPoints(std::forward_list<glm::vec3>* attractionPoints)
{
	//Determine crown envelope
	//for this test, our crown envelope will be a simple upper hemisphere centered on 0, 10, 0, radius 10
	glm::vec3 crownCenter = glm::vec3(0.0f, species->crownCenterHeight, 0.0f);	//if this is exactly 0,y,0 we get a twisting as the starting vertex for the ring gets generated at 90deg off
	//generate attraction points in the envelope based on its shape
	switch (species->crownShape)
	{
	case HEMISPHERE:
		for (int i = 0; i < species->attractionPointCount; i++)
		{
			glm::vec3 randomPoint = MeshTools::randomPointInUnitSphere();
			if (randomPoint.y < 0.0f)
			{
				randomPoint.y *= -1.0f;	//if the point was generated in the lower half of the sphere, invert y to move it into the upper half
			}
			attractionPoints->push_front(crownCenter + (randomPoint * species->crownRadius));
		}
		break;
	case SPHERE:
		for (int i = 0; i < species->attractionPointCount; i++)
		{
			glm::vec3 randomPoint = MeshTools::randomPointInUnitSphere();
			attractionPoints->push_front(crownCenter + (randomPoint * species->crownRadius));
		}
		break;
	case CONE:
		for (int i = 0; i < species->attractionPointCount; i++)
		{
			glm::vec2 circlePoint = MeshTools::randomPointInUnitCircle();
			float randomHeight = RandomFloat() * species->crownHeight;
			//float radiusAtHeight = (randomHeight / species->crownHeight) * species->crownRadius;
			float radiusAtHeight = randomHeight * (species->crownRadius / species->crownHeight);
			glm::vec3 pointInCone = radiusAtHeight * glm::vec3(circlePoint.x, 1.0f, circlePoint.y);
			attractionPoints->push_front(crownCenter + pointInCone);
		}
		break;
	}
}

void Tree::addTreeNodeToMeshRecursive(TreeNode* node)
{
	TreeNode* parent = node->parent;
	glm::vec3 pVector = glm::normalize(node->location - parent->location);
	glm::vec3 parentsPVector = glm::vec3(0.0f, -1.0f, 0.0f);
	if (parent->parent != nullptr)
	{
		parentsPVector = glm::normalize(parent->location - parent->parent->location);
	}

	if (node->children.size() > 0)		//not a branch tip
	{
		//if the p-vector and parents p-vectors are similar, we just link directly to the parent's vertex ring
		//otherwise, we create a new vertex ring and link to that
		if (glm::length(pVector - parentsPVector) < species->branchingThreshold)
		{
			MeshTools::addVertexRing(&vertices, node->location, pVector, node->radius, TREE_SIDES);
			node->indexOfLastVertex = vertices.size() - 1;
			MeshTools::linkVertexRingsAtPosition(&vertices, &indices, TREE_SIDES, node->indexOfLastVertex, node->parent->indexOfLastVertex);
		}
		else
		{
			MeshTools::addVertexRing(&vertices, parent->location, pVector, node->radius, TREE_SIDES);
			MeshTools::addVertexRing(&vertices, node->location, pVector, node->radius, TREE_SIDES);
			node->indexOfLastVertex = vertices.size() - 1;
			MeshTools::linkVertexRings(&vertices, &indices, TREE_SIDES);
		}
		//now recursively do the same for the children
		for (auto& child : node->children)
		{
			addTreeNodeToMeshRecursive(child);
		}
	}
	else
	{
		//this node is a branch tip. End the branch with a point and stop recursing.
		//if the p-vector and parents p-vectors are similar, we just link directly to ther parent's vertex ring
		//otherwise, we create a new vertex ring and link to that
		if (glm::length(pVector - parentsPVector) < species->branchingThreshold)
		{
			MeshTools::ringToPointAtPosition(&vertices, &indices, node->location, TREE_SIDES, node->parent->indexOfLastVertex);
		}
		else
		{
			MeshTools::addVertexRing(&vertices, parent->location, pVector, node->radius, TREE_SIDES);
			MeshTools::ringToPoint(&vertices, &indices, node->location, TREE_SIDES);
		}

		//next, we add some leaves to the end of the branch
		//lets try a simple cone
		MeshTools::addVertexRing(&leavesVertices, parent->location, parentsPVector, species->leavesRadius, TREE_SIDES);
		MeshTools::ringToPoint(&leavesVertices, &leavesIndices, node->location, TREE_SIDES);
		
	}
}

float Tree::recursivelyCalculateWidths(TreeNode* node)
{
	if (node->children.size() == 0)
	{
		//this is an end node
		node->radius = 0.0f;
		return 0.0f;
	}
	else if (node->children.size() == 1)
	{
		//not at a fork, width is slightly thicker than previous node
		float childArea = recursivelyCalculateWidths(node->children[0]);
		node->radius = glm::sqrt(childArea / glm::pi<float>()) + species->thicknessModifier;
		return glm::pi<float>() * node->radius * node->radius;
	}
	else
	{
		//width of this node is such that its area is the sum of the areas of its children
		float totalChildArea = 0.0f;
		for (auto& child : node->children)
		{
			totalChildArea += recursivelyCalculateWidths(child);
		}
		//set the radius of this node
		node->radius = glm::sqrt(totalChildArea / glm::pi<float>());
		//and return the area to be used for calcuations of its parents
		return totalChildArea;
	}
}