#ifndef SHADER_H
#define SHADER_H

#include "GLdef.hpp"

#include <vector>
#include <string>
#include <unordered_map>

namespace Window
{

class Shader
{
public:
	Shader();
	virtual ~Shader();
	bool addVertexShader(std::string text);
	bool addFragmentShader(std::string text);
	virtual bool compile();
	virtual void bind();
	virtual void unbind();
	bool setParameter(std::string name, float val);
	bool setParameter(std::string name, int val);
	bool setParameter(std::string name, bool val);

protected:
	bool addProgram(std::string text, GLenum type);
	void addAllUniform();
	GLint getUniform(std::string name);
	std::vector<GLuint> shaderList;
	std::unordered_map<std::string,GLint> uniforms;
	GLuint program;
};
}

#endif
