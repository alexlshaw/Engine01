#ifndef ENGINE01_GLUTILS_H
#define ENGINE01_GLUTILS_H

class GLUtils
{
public:
    GLUtils();

    static int checkForOpenGLError(const char *, int);
    static void dumpGLInfo(bool dumpExtensions = false);
};

#endif
