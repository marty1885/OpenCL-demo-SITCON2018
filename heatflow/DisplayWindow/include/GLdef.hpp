#ifndef GLDEF_H
#define GLDEF_H

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
#elif defined __ANDROID__
	#include <EGL/egl.h>
    #include <EGL/eglext.h>
	#include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays;
    extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray;
    extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays;
#else
	#include <GL/glew.h>
	#include <GL/gl.h>
#endif

#endif
