#ifndef ENGINE01_TREE_H
#define ENGINE01_TREE_H

#include <deque>
#include <forward_list>
#include <vector>
#include "glad/glad.h"
#include "glm\glm.hpp"
#include "glm\gtx\rotate_vector.hpp"

#include "Camera.h"
#include "DebuggingTools.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "Settings.h"
#include "Shader.h"
#include "Transform.h"
#include "TreeSpecies.h"
#include "Vertex.h"

struct TreeNode
{
	TreeNode* parent;
	glm::vec3 location;
	std::vector<glm::vec3> influenceVectors;
	std::deque<TreeNode*> children;
	int indexOfLastVertex;
	float radius;
};

class Tree : public GameObject
{
private:
	TreeSpecies* species;
	Mesh* mesh;
	Mesh* leavesMesh;
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<ColouredVertex> leavesVertices;
	std::vector<unsigned int> leavesIndices;
	void buildTreeSpaceColonizationMethod();
	void generateAttractionPoints(std::forward_list<glm::vec3>* attractionPoints);
	void addTreeNodeToMeshRecursive(TreeNode* node);
	float recursivelyCalculateWidths(TreeNode* node);
public:
	Tree(TreeSpecies* species);
	~Tree();
	void draw(int renderPass, Transform* transform);
};

#endif