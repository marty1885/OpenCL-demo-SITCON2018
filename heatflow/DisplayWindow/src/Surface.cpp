#include <Surface.hpp>
using namespace Window;

Surface::Surface()
{
	float coord[] = {-1,-1, -1,1, 1,1, 1,-1};
	unsigned int indices[] = {0,1,2,2,3,0};
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,4*2*sizeof(float),coord,GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glGenBuffers(1,&ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,6*sizeof(int),indices,GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

	// Create one OpenGL texture
	glGenTextures(1, &textureID);
}

Surface::~Surface()
{
	if(textureID != 0)
		glDeleteTextures(1,&textureID);
	glDeleteBuffers(1,&vbo);
	glDeleteBuffers(1,&ebo);
	glDeleteVertexArrays(1,&vao);
}

void Surface::render()
{
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Surface::setTexture(const unsigned char* buffer, int width, int height)
{
	glActiveTexture(GL_TEXTURE0);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Surface::setTexture(const float* buffer, int width, int height)
{
	glActiveTexture(GL_TEXTURE0);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, buffer);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}
