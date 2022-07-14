#ifndef ENGINE01_DEBUGGINGTOOLS_H
#define ENGINE01_DEBUGGINGTOOLS_H

#include <iostream>
#include "glad/glad.h"

//Set up our debug printing tools
#ifdef _DEBUG
#define DEBUG_PRINT(x) printf(x)
#define DEBUG_PRINT_GL_ERRORS() printGLErrors()
#define DEBUG_WAIT_FOR_CONSOLE_INPUT() char debugWaitForConsoleInputChar = getchar()
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINT_GL_ERRORS()
#define DEBUG_WAIT_FOR_CONSOLE_INPUT()
#endif

inline void printGLErrors()
{
#ifdef _DEBUG
	GLenum flag = glGetError();
	if (flag != GL_NO_ERROR)
	{
		switch (flag)
		{
		case GL_INVALID_ENUM:
			DEBUG_PRINT("GL_INVALID_ENUM\n");
			break;
		case GL_INVALID_VALUE:
			DEBUG_PRINT("GL_INVALID_VALUE\n");
			break;
		case GL_INVALID_OPERATION:
			DEBUG_PRINT("GL_INVALID_OPERATION\n");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			DEBUG_PRINT("GL_INVALID_FRAMEBUFFER_OPERATION\n");
			break;
		case GL_OUT_OF_MEMORY:
			DEBUG_PRINT("GL_OUT_OF_MEMORY\n");
			break;
		//case GL_STACK_UNDERFLOW:
		//	DEBUG_PRINT("GL_STACK_UNDERFLOW\n");
		//	break;
		//case GL_STACK_OVERFLOW:
		//	DEBUG_PRINT("GL_STACK_OVERFLOW\n");
		//	break;
		default:
			DEBUG_PRINT("An OpenGL error with an unknown code occured.\n");
			break;
		}
	}
#endif
}

#endif