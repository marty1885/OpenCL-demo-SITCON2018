#ifndef GLSLSOURCE_H
#define GLSLSOURCE_H

namespace Window
{
//FIXME: Gamma correction using Intel/AMD/nvidia/(and maby other) GPUs make weird color in daek reagons

#ifdef __ANDROID__

const char basic2DVertexShaderSource[] =
"attribute vec2 position; \n\
\n\
varying vec2 textureCoord;\n\
\n\
void main()\n\
{\n\
	gl_Position = vec4(position,0,1.0);\n\
	textureCoord = position*0.5 + vec2(0.5);\n\
}\n\n";

const char gammaCorrectionFragmantShaderSource[] =
"precision mediump float; \n\
\n\
varying vec2 textureCoord;\n\
uniform sampler2D textureSampler;\n\
uniform float gamma;\n\
\n\
void main()\n\
{\n\
	vec4 uncorrectedColor = texture2D(textureSampler,vec2(textureCoord.x,-textureCoord.y));\n\
	float inverseGamma = 1.0f/gamma;\n\
	vec4 correctedColor = pow(uncorrectedColor,vec4(inverseGamma,inverseGamma,inverseGamma,0));\n\
	gl_FragColor = correctedColor;\n\
}\n\n";

#else

const char basic2DVertexShaderSource[] =
"#version 330 core\n\
\n\
layout (location = 0) in vec2 position;\n\
\n\
out vec2 textureCoord;\n\
\n\
void main()\n\
{\n\
	gl_Position = vec4(position,0,1.0);\n\
	textureCoord = position*0.5 + vec2(0.5);\n\
}\n\n";

const char gammaCorrectionFragmantShaderSource[] =
"#version 330 core\n\
out vec4 outColor;\n\
\n\
in vec2 textureCoord;\n\
uniform sampler2D textureSampler;\n\
uniform float gamma;\n\
\n\
vec4 getColor(float hMax, float hMin, float t)\n\
{\n\
    t = clamp(t, hMin, hMax);\n\
    float invMaxH = 1.0f / (hMax - hMin);\n\
    float zRel = (t - hMin) * invMaxH;\n\
    float cR = 0, cG = 0, cB = 0;\n\
    if (0 <= zRel && zRel < 0.2f)\n\
    {\n\
        cB = 1.0f;\n\
        cG = zRel * 5.0f;\n\
    }\n\
    else if (0.2f <= zRel && zRel < 0.4f)\n\
    {\n\
        cG = 1.0f;\n\
        cB = 1.0f - (zRel - 0.2f) * 5.0f;\n\
    }\n\
    else if (0.4f <= zRel && zRel < 0.6f)\n\
    {\n\
        cG = 1.0f;\n\
        cR = (zRel - 0.4f) * 5.0f;\n\
    }\n\
    else if (0.6f <= zRel && zRel < 0.8f)\n\
    {\n\
        cR = 1.0f;\n\
        cG = 1.0f - (zRel - 0.6f) * 5.0f;\n\
    }\n\
    else\n\
    {\n\
        cR = 1.0f;\n\
        cG = (zRel - 0.8f) * 5.0f;\n\
        cB = cG;\n\
    }\n\
    return vec4(cR, cG, cB, 1.0f);\n\
}\n\
\n\
void main()\n\
{\n\
	vec4 uncorrectedColor = texture(textureSampler,vec2(textureCoord.x,-textureCoord.y));\n\
	uncorrectedColor = getColor(1, 0 ,uncorrectedColor.r);\n\
	float inverseGamma = 1.0f/gamma;\n\
	vec4 correctedColor = pow(uncorrectedColor,vec4(inverseGamma,inverseGamma,inverseGamma,0));\n\
	outColor = correctedColor;\n\
}\n\n";

#endif

}

#endif
