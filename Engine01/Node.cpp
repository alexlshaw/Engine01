#include "Node.h"

Node::Node()
{
	heightMapTexture = -1;
	splatMapTexture = -1;
	state = UNBUILT;
}

Node::~Node()
{
	clear(); //clear releases resources, so it's easiest to just call it when we destroy the node
}

void Node::clear()
{
	//return the node to the unbuilt state
	if (heightMapTexture != -1)
	{
		glDeleteTextures(1, &heightMapTexture);
	}
	if (splatMapTexture != -1)
	{
		glDeleteTextures(1, &splatMapTexture);
	}
	state = UNBUILT;
	heightMapTexture = -1;
	splatMapTexture = -1;
}

void Node::pushHeightmap()
{
	if (state == BUILT)
	{
		//push the texture to the graphics card
		glGenTextures(1, &heightMapTexture);
		glBindTexture(GL_TEXTURE_2D, heightMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, HEIGHTMAP_TEXTURE_WIDTH, HEIGHTMAP_TEXTURE_WIDTH, 0, GL_RED, GL_FLOAT, (GLvoid*)&heightMapData[0]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Node::pushSplatMap()
{
	if (state == HM_PUSHED)
	{
		glGenTextures(1, &splatMapTexture);
		glBindTexture(GL_TEXTURE_2D, splatMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, HEIGHTMAP_TEXTURE_WIDTH, HEIGHTMAP_TEXTURE_WIDTH, 0, GL_RGBA, GL_FLOAT, (GLvoid*)&splatMapData[0]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}