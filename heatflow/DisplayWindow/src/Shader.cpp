#include <Shader.hpp>
#include "Log.h"

#include <iostream>

using namespace std;
using namespace Window;

Shader::Shader()
{
	program = glCreateProgram();
	if(program == 0)
		LOGI("Failed to create shader.\n");
}

Shader::~Shader()
{
	glDeleteProgram(program);

	int size = shaderList.size();
	for(int i=0;i<size;i++)
		glDeleteShader(shaderList[i]);
	shaderList.clear();
}

bool Shader::addVertexShader(string text)
{
	return addProgram(text, GL_VERTEX_SHADER);
}

bool Shader::addFragmentShader(string text)
{
	return addProgram(text, GL_FRAGMENT_SHADER);
}

bool Shader::compile()
{
	glLinkProgram(program);

	int result;
	glGetProgramiv(program,GL_LINK_STATUS,&result);
	if(result != GL_TRUE)
	{
		char* data = new char[2001];
		int length;
		glGetProgramInfoLog(program,2000,&length,data);
		LOGI("Failed to link program: %s\n", data);
		delete [] data;
		return false;
	}

	glValidateProgram(program);
	glGetProgramiv(program,GL_VALIDATE_STATUS,&result);
	if(result == 0)
	{
		char* data = new char[2001];
		int length;
		glGetProgramInfoLog(program,2000,&length,data);
		LOGI("Failed to validate program: %s\n", data);
		delete [] data;
		return false;
	}
	//cache uniforms in a hash table for fast access
	addAllUniform();
	return true;
}

void Shader::bind()
{
	glUseProgram(program);
}

void Shader::unbind()
{
	glUseProgram(0);
}

bool Shader::addProgram(string text, GLenum type)
{

	//Create vertex shader
	GLuint shader = glCreateShader(type);
	const char* str = text.c_str();

	if(shader == 0)
	{
		LOGI("Failed to create shader.\n");
		return false;
	}

	//Set vertex source
	glShaderSource(shader, 1, &str, NULL);

	//Compile vertex source
	glCompileShader(shader);

	//Check vertex shader for errors
	GLint result = GL_FALSE;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &result );
	if(result != GL_TRUE)
	{
		char* data = new char[2001];
		int length;
		glGetShaderInfoLog(shader,2000,&length,data);
		string shaderType = "Unknown";
		switch (type)
		{
			case GL_VERTEX_SHADER:
				shaderType = "vertex";
				break;
			case GL_FRAGMENT_SHADER:
				shaderType = "fragment";
				break;
			default:
				shaderType = "Unknown";
		}
		LOGI("Failed to compile %s shader: %s\n", shaderType.c_str(), data);
		delete [] data;
		return false;
	}
	//Attach vertex shader to program
	glAttachShader(program, shader);
	shaderList.push_back(shader);
	return true;
}

bool Shader::setParameter(string name,float val)
{
	//GLint uniformLocation = glGetUniformLocation(program,name.c_str());
	GLint uniformLocation = getUniform(name);
	if(uniformLocation == -1)
	{
		LOGI("Error : Uniform \" %s \" not found.\n", name.c_str());
		return false;
	}
	glUniform1f(uniformLocation,val);
	return true;
}

bool Shader::setParameter(string name, bool val)
{
	GLint uniformLocation = getUniform(name);
	if(uniformLocation == -1)
	{
		LOGI("Error : Uniform \" %s \" not found.\n", name.c_str());
		return false;
	}
	glUniform1i(uniformLocation,val);
	return true;
}

bool Shader::setParameter(string name, int val)
{
	GLint uniformLocation = getUniform(name);
	if(uniformLocation == -1)
	{
		LOGI("Error : Uniform \" %s \" not found.\n", name.c_str());
		return false;
	}
	glUniform1i(uniformLocation,val);
	return true;
}

void Shader::addAllUniform()
{
	int total = -1;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &total);

	int maxLength;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
	char* name = new char[maxLength];
	for(int i=0; i<total; i++)
	{
		int nameLen=-1, num=-1;
		GLenum type = GL_ZERO;
		glGetActiveUniform(program, GLuint(i), maxLength,
			&nameLen, &num, &type, name);
		name[nameLen] = 0;
		GLuint location = glGetUniformLocation(program, name);
		uniforms.insert({name,location});
	}
	delete [] name;
}

GLint Shader::getUniform(string name)
{
	unordered_map<string,GLint>::const_iterator search = uniforms.find(name);
	if(search == uniforms.end())
		return -1;
	return search->second;
}
