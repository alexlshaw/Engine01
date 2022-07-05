#ifndef ENGINE01_ROCKTYPE_H
#define ENGINE01_ROCKTYPE_H

#include <fstream>
#include <map>
#include <string>

#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include "Material.h"

class RockType
{
private:
	void setFields(std::map<std::string, std::string> keyValues, GraphicsResourceManager* resourceManager);
public:
	RockType(const char* dataFile, GraphicsResourceManager* resourceManager);
	int pointCount;
	float pointRadius;
	float scatterRadius;
	float pointYScaling;
	Material* material;
};

#endif