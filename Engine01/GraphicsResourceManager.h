#ifndef ENGINE01_GRAPHICSRESOURCEMANAGER_H
#define ENGINE01_GRAPHICSRESOURCEMANAGER_H

#include <algorithm>
#include <fstream>
#include <map>
#include <string>

#include "Material.h"
#include "Shader.h"
#include "Texture.h"

class GraphicsResourceManager
{
private:
	std::map<std::string, Material*> materials;
	std::map<std::string, Shader*> shaders;
	std::map<std::string, Texture*> textures;
	Material* loadMaterialFromFile(std::string fileName);
	Shader* loadShaderFromFile(std::string fileName);
	Texture* loadTextureFromFile(std::string fileName);
public:
	GraphicsResourceManager();
	~GraphicsResourceManager();
	Material* loadMaterial(std::string name);
	Shader* loadShader(std::string name);		//Note that at the moment we can only load the standard Vertex+Fragment shader type
	Texture* loadTexture(std::string name);
};

#endif