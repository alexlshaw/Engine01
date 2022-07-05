#include "Text2D.hpp"

static const char * texturePath = "Data/Holstein.tga";
GLint textureUniformLoc;
unsigned int Text2DTextureID;
unsigned int Text2DVertexBufferID;
unsigned int Text2DUVBufferID;
Shader text2dProgram;
GLuint screenVectorLoc;
GLuint vao_text2d;
glm::vec2 screenVector;

void initText2D(int screenWidth, int screenHeight)
{
	screenVector = glm::vec2((float)screenWidth / 2.0f, (float)screenHeight / 2.0f);
	// Initialize texture
	Text2DTextureID = 0;
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;

	pBits = readTGABits(texturePath, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == nullptr) 
	{
		return;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &Text2DTextureID);
	glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
	free(pBits);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Initialize VBO
	glGenVertexArrays(1, &vao_text2d);
	glGenBuffers(1, &Text2DVertexBufferID);
	glGenBuffers(1, &Text2DUVBufferID);
	glBindVertexArray(vao_text2d);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	text2dProgram.compileShaderFromFile("./Data/Shaders/ui/text2d.vert", VERTEX); 
	text2dProgram.compileShaderFromFile("./Data/Shaders/ui/text2d.frag", FRAGMENT);
	text2dProgram.link();
	textureUniformLoc = text2dProgram.getUniformLocation("font");
	screenVectorLoc = text2dProgram.getUniformLocation("screenVector");
}

void printText2D(const char * text, int x, int y, int size)
{
	unsigned int length = strlen(text);

	// Fill buffers
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;
	for ( unsigned int i=0 ; i<length ; i++ ){
		float letterWidth = 0.5f * i;
		glm::vec2 vertex_up_left    = glm::vec2( x+letterWidth*size     , y+size );
		glm::vec2 vertex_up_right   = glm::vec2( x+letterWidth*size+size, y+size );
		glm::vec2 vertex_down_right = glm::vec2( x+letterWidth*size+size, y      );
		glm::vec2 vertex_down_left  = glm::vec2( x+letterWidth*size     , y      );

		vertices.push_back(vertex_up_left   );
		vertices.push_back(vertex_down_left );
		vertices.push_back(vertex_up_right  );

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character%16)/16.0f;
		float uv_y = (character/16)/16.0f;

		glm::vec2 uv_up_left    = glm::vec2( uv_x           , 1.0f - uv_y );
		glm::vec2 uv_up_right   = glm::vec2( uv_x+1.0f/16.0f, 1.0f - uv_y );
		glm::vec2 uv_down_right = glm::vec2( uv_x+1.0f/16.0f, 1.0f - (uv_y + 1.0f/16.0f) );
		glm::vec2 uv_down_left  = glm::vec2( uv_x           , 1.0f - (uv_y + 1.0f/16.0f) );
		UVs.push_back(uv_up_left   );
		UVs.push_back(uv_down_left );
		UVs.push_back(uv_up_right  );

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}

	glBindVertexArray(vao_text2d);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STREAM_DRAW);

	// Bind shader
	text2dProgram.use();

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	text2dProgram.setUniform(textureUniformLoc, 0);
	text2dProgram.setUniform(screenVectorLoc, screenVector);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisable(GL_BLEND);
	glBindVertexArray(0);
}

void cleanupText2D(){
	text2dProgram.~Shader();
	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

	// Delete texture
	glDeleteTextures(1, &Text2DTextureID);

	// Delete shader
	glDeleteVertexArrays(1, &vao_text2d);
}
