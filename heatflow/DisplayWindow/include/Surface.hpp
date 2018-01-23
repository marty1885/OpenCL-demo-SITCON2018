#ifndef SURFACE_H
#define SURFACE_H

#include "GLdef.hpp"

namespace Incarnate
{
class Film;
}


namespace Window
{

class Surface
{
public:
	Surface();
	virtual ~Surface();
	void setTexture(const float* buffer, int width, int height);
	void setTexture(const unsigned char* buffer, int width, int height);

	void render();
protected:
	GLuint textureID = 0;

	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
};
}

#endif
