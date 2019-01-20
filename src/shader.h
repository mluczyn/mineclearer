#pragma once
#include <GL/glew.h>

#define GLSL(src) "#version 440 core\n" #src

GLchar* vertexSource = GLSL(
layout(location = 0) in vec2 pos;
layout(location = 1) in uint texIndx;

out VS_OUT
{
	uint texIndex;
} vs_out;


void main()
{
	vs_out.texIndex = texIndx;
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}
);

GLchar* geometrySource = GLSL(
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in VS_OUT
{
	uint texIndex;
} gs_in[];
out vec3 texPos;

uniform vec2 tileDim;

void main()
{
	vec4 position = gl_in[0].gl_Position;
	gl_Position = position + vec4(0.0, 0.0, 0.0, 1.0);
	texPos = vec3(0, 1, gs_in[0].texIndex);
	EmitVertex();
	gl_Position = position + vec4(0.0, tileDim.y, 0.0, 1.0);
	texPos = vec3(0, 0, gs_in[0].texIndex);
	EmitVertex();
	gl_Position = position + vec4(tileDim.x, 0, 0.0, 1.0);
	texPos = vec3(1, 1, gs_in[0].texIndex);
	EmitVertex();
	gl_Position = position + vec4(tileDim.x, tileDim.y, 0.0, 1.0);
	texPos = vec3(1, 0, gs_in[0].texIndex);
	EmitVertex();

	EndPrimitive();
}
);

GLchar* fragmentSource = GLSL(
in vec3 texPos;
out vec4 color;

uniform sampler2DArray texArray;

void main()
{
	color = texture(texArray, texPos);
}
);

GLuint compileShader(GLchar** source, GLenum shadertype, const char* name)
{
	GLuint shaderh = glCreateShader(shadertype);
	glShaderSource(shaderh, 1, source, NULL);
	glCompileShader(shaderh);
	GLint result;
	glGetShaderiv(shaderh, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		GLchar log[512];
		glGetShaderInfoLog(shaderh, 512, NULL, log);
		printf("%s Shader Compilation Error:%s\n", name, log);
	}
	return shaderh;
}