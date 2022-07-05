#include "RockType.h"

RockType::RockType(const char* dataFile, GraphicsResourceManager* resourceManager)
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

void RockType::setFields(std::map<std::string, std::string> keyValues, GraphicsResourceManager* resourceManager)
{
	std::map<std::string, std::string>::iterator it;
	if ((it = keyValues.find("pointCount")) != keyValues.end())
	{
		pointCount = std::stoi(it->second);
	}
	if ((it = keyValues.find("pointRadius")) != keyValues.end())
	{
		pointRadius = std::stof(it->second);
	}
	if ((it = keyValues.find("scatterRadius")) != keyValues.end())
	{
		scatterRadius = std::stof(it->second);
	}
	if ((it = keyValues.find("pointYScaling")) != keyValues.end())
	{
		pointYScaling = std::stof(it->second);
	}
	if ((it = keyValues.find("material")) != keyValues.end())
	{
		material = resourceManager->loadMaterial(it->second);
	}
}