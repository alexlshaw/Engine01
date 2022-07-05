#ifndef ENGINE01_TREESPECIES_H
#define ENGINE01_TREESPECIES_H

#include <fstream>
#include <map>
#include <string>

#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include "Material.h"

const static enum CrownShape
{
	HEMISPHERE,
	SPHERE,
	CONE
};

class TreeSpecies
{
private:
	void setFields(std::map<std::string, std::string> keyValues, GraphicsResourceManager* resourceManager);
public:
	TreeSpecies(const char* dataFile, GraphicsResourceManager* resourceManager);
	float thicknessModifier;
	float branchingThreshold;
	float leavesRadius;
	float heightToFirstBranches;
	float crownCenterHeight;
	float crownRadius;
	float crownHeight;
	CrownShape crownShape;
	float newNodeDistance;
	int attractionPointCount;
	float radiusOfInfluence;
	float killDistance;
	int maxIterations;
	Material* barkMaterial;
	Material* leavesMaterial;
	std::string biomesTag;
};

#endif