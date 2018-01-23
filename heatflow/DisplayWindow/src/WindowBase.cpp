#include <stdio.h>
#include <string.h>

#include <string>
#include <iostream>
using namespace std;

#include <Window.hpp>
#include <GLSLSource.hpp>
#include <Shader.hpp>
#include <Surface.hpp>

using namespace Incarnate;

WindowBase::WindowBase()
{
}

WindowBase::~WindowBase()
{
}

void WindowBase::generateRenderTexture(const unsigned char* buffer, int width, int height)
{
	surface->setTexture(buffer,width,height);
}

void WindowBase::generateRenderTexture(const float* buffer, int width, int height)
{
	surface->setTexture(buffer,width,height);
}

void WindowBase::render()
{
	int width = getWidth();
	int height = getHeight();

	glViewport(0,0,width,height);

	shader->bind();
	surface->render();
}

void WindowBase::setGamma(float gamma)
{
	shader->setParameter("gamma",gamma);
}

bool WindowBase::initGL()
{
	#if !defined(__APPLE__) && !defined(__ANDROID__)
		glewExperimental = GL_TRUE;
		GLuint err = glewInit();
		if(err != GLEW_OK)
		{
			cout << "Failed to init GLEW. Error : " << glewGetErrorString(err) << endl;
			return false;
		}
		glGetError(); //Jump trough the GL_INVALIUD_ENUM created my glewInit()
	#endif

	//Setup OpenGL state
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	surface = new Window::Surface;
	shader = new Window::Shader;
	bool success = true;

	success &= shader->addVertexShader(Window::basic2DVertexShaderSource);
	success &= shader->addFragmentShader(Window::gammaCorrectionFragmantShaderSource);
	success &= shader->compile();

	if(success == false)
		return false;

	shader->bind();
	shader->setParameter("gamma",2.2f);
	return true;
}

void WindowBase::terminateGL()
{
	delete surface;
	delete shader;
	surface = NULL;
	shader = NULL;
}
