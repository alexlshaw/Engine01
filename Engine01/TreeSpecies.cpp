#include "TreeSpecies.h"

TreeSpecies::TreeSpecies(const char* dataFile, GraphicsResourceManager* resourceManager)
{
	std::string line;
	std::ifstream fs(dataFile);
	std::map<std::string, std::string> keyValues;
	while (std::getline(fs, line))
	{
		if (!line.empty() && line.at(0) != '#')	//lines starting with # are comments
		{
			std::string varName = line.substr(0, line.find('='));
			std::string varValue = line.substr(line.find('=') + 1, line.length());
			keyValues.emplace(varName, varValue);
		}
	}
	fs.close();
	setFields(keyValues, resourceManager);

}

void TreeSpecies::setFields(std::map<std::string, std::string> keyValues, GraphicsResourceManager* resourceManager)
{
	std::map<std::string, std::string>::iterator it;
	if ((it = keyValues.find("thicknessModifier")) != keyValues.end())
	{
		thicknessModifier = std::stof(it->second);
	}
	if ((it = keyValues.find("branchingThreshold")) != keyValues.end())
	{
		branchingThreshold = std::stof(it->second);
	}
	if ((it = keyValues.find("leavesRadius")) != keyValues.end())
	{
		leavesRadius = std::stof(it->second);
	}
	if ((it = keyValues.find("heightToFirstBranches")) != keyValues.end())
	{
		heightToFirstBranches = std::stof(it->second);
	}
	if ((it = keyValues.find("crownCenterHeight")) != keyValues.end())
	{
		crownCenterHeight = std::stof(it->second);
	}
	if ((it = keyValues.find("crownRadius")) != keyValues.end())
	{
		crownRadius = std::stof(it->second);
	}
	if ((it = keyValues.find("crownShape")) != keyValues.end())
	{
		if (it->second == "HEMISPHERE")
		{
			crownShape = HEMISPHERE;
		}
		else if (it->second == "SPHERE")
		{
			crownShape = SPHERE;
		}
		else if (it->second == "CONE")
		{
			crownShape = CONE;
			if ((it = keyValues.find("crownHeight")) != keyValues.end())
			{
				crownHeight = std::stof(it->second);
			}
		}
	}
	if ((it = keyValues.find("newNodeDistance")) != keyValues.end())
	{
		newNodeDistance = std::stof(it->second);
	}
	if ((it = keyValues.find("attractionPointCount")) != keyValues.end())
	{
		attractionPointCount = std::stoi(it->second);
	}
	if ((it = keyValues.find("radiusOfInfluence")) != keyValues.end())
	{
		radiusOfInfluence = std::stof(it->second);
	}
	if ((it = keyValues.find("killDistance")) != keyValues.end())
	{
		killDistance = std::stof(it->second);
	}
	if ((it = keyValues.find("maxIterations")) != keyValues.end())
	{
		maxIterations = std::stoi(it->second);
	}
	if ((it = keyValues.find("barkMaterial")) != keyValues.end())
	{
		barkMaterial = resourceManager->loadMaterial(it->second);
	}
	if ((it = keyValues.find("leavesMaterial")) != keyValues.end())
	{
		leavesMaterial = resourceManager->loadMaterial(it->second);
	}
	if ((it = keyValues.find("biomes")) != keyValues.end())
	{
		biomesTag = it->second;
	}
}