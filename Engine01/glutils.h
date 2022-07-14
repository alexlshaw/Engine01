#ifndef ENGINE01_GLUTILS_H
#define ENGINE01_GLUTILS_H

#include <cstdio>
#include "glad/glad.h"

namespace GLUtils
{
    int checkForOpenGLError(const char *, int);
    void dumpGLInfo(bool dumpExtensions = false);
}

#endif
