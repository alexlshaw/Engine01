#ifndef ENGINE01_TEXT2D_HPP
#define ENGINE01_TEXT2D_HPP

#include <vector>
#include <cstring>
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Texture.h"

void initText2D(int screenWidth, int screenHeight);
void printText2D(const char * text, int x, int y, int size);
void cleanupText2D();

#endif