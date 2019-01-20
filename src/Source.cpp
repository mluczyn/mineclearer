#include <cstdio>
#include <cstring>
#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

//Opengl Headers
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL\SOIL.h>
#include "shader.h"
#include "VBO.h"
#include "mineField.h"

//Tile States
enum TState
{
	covered,
	uncovered,
	flaged
};

bool gameComplete = false, minefieldActive = false;
int fieldWidth = 20, fieldHeight = 15, mines = 150, flagCounter = 0, INDEX_MAX, TEXID_OFFSET;
int WIDTH = 40 * fieldWidth, HEIGHT = 40 * fieldHeight;
Minefield minefield(fieldWidth, fieldHeight);
GLFWwindow* window;
GLubyte *tileState, FLAG_VAL = 10, COVER_VAL = 9, MINE_VAL = 8;
GLfloat tileWidth, tileHeight;
GLuint shaderProg, VAO, texArray;
GLint dimLoc;
std::shared_ptr<PersistentVBO> VBO;

GLuint getTileIndex(double x, double y)
{
	if (x < 0.0 || y < 0.0 || x >(double)WIDTH || y >(double)HEIGHT)
		return INDEX_MAX;
	else
		return fieldWidth * (fieldHeight - (int)(fieldHeight * y / HEIGHT)) + (int)(fieldWidth * x / WIDTH) - fieldWidth;
}

void uncoverTile(GLuint tile)
{
	
}

void uncoverAdjacent(GLuint tile)
{
	
}

void onLeftClick(GLuint tile)
{
	if (tileState[tile] == TState::covered)
		uncoverTile(tile);
	else if (tileState[tile] == TState::uncovered)
		uncoverAdjacent(tile);
}

void toggleFlag(GLuint tile)
{
	
}

void clickCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (gameComplete)
		{
			glfwSetWindowShouldClose(window, true);
			return;
		}

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		int index = getTileIndex(x, y);
		if (index < INDEX_MAX)
		{
			if (!minefieldActive)
			{
				minefield.genField(index, mines);
				minefieldActive = true;
			}
			if (button == GLFW_MOUSE_BUTTON_LEFT)
			{
				if (minefield.getTileVal(index) == 8 && tileState[index] != TState::flaged)
				{
					printf("You Died\n");
					gameComplete = true;
				}
				onLeftClick(index);
			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT)
				toggleFlag(index);
		}
	}
}

int main()
{
	INDEX_MAX = fieldWidth * fieldHeight;
	TEXID_OFFSET = 8 * INDEX_MAX;
	tileState = new GLubyte[INDEX_MAX];
	memset(tileState, TState::covered, INDEX_MAX);

	{/*Init*/}
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "Minesweeper", nullptr, nullptr);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetMouseButtonCallback(window, clickCallback);
		glfwMakeContextCurrent(window);

		glewExperimental = 1;
		glewInit();

		glViewport(0, 0, WIDTH, HEIGHT);
	}
	{/*Shaders*/}
	{
		shaderProg = glCreateProgram();
		GLuint vShader, gShader, fShader;
		vShader = compileShader(&vertexSource, GL_VERTEX_SHADER, "Vertex");
		gShader = compileShader(&geometrySource, GL_GEOMETRY_SHADER, "Geometry");
		fShader = compileShader(&fragmentSource, GL_FRAGMENT_SHADER, "Fragment");
		glAttachShader(shaderProg, vShader);
		glAttachShader(shaderProg, gShader);
		glAttachShader(shaderProg, fShader);
		glLinkProgram(shaderProg);
		GLint result;
		glGetProgramiv(shaderProg, GL_LINK_STATUS, &result);
		if (result == GL_FALSE)
		{
			GLchar log[512];
			glGetProgramInfoLog(shaderProg, 512, NULL, log);
			printf("Shader Program Linkage Error:%s\n", log);
		}
		glDeleteShader(vShader);
		glDeleteShader(gShader);
		glDeleteShader(fShader);
		glUseProgram(shaderProg);

		tileWidth = 4.0 * 0.9 / fieldWidth;
		tileHeight = 4.0 * 0.9 / fieldHeight;
		dimLoc = glGetUniformLocation(shaderProg, "tileDim");
	}

	{/*Buffers*/}
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		VBO = std::shared_ptr<PersistentVBO>(new PersistentVBO(9 * INDEX_MAX));
		VBO->setTextureOffset(TEXID_OFFSET);
 
		double xStep = 4.0 / fieldWidth;
		double xOff = 2.0 - xStep / 20.0;
		double yStep = 4.0 / fieldHeight;
		double yOff = 2.0 - yStep / 20.0;
		
		GLubyte* VBObuf = new GLubyte[9 * INDEX_MAX];
		for (size_t i = 0; i < INDEX_MAX; ++i)
		{
			((GLfloat*)VBObuf)[i * 2] = (i % fieldWidth) * xStep - xOff;
			((GLfloat*)VBObuf)[i * 2 + 1] = floor(i / fieldWidth) * yStep - yOff;
			VBObuf[TEXID_OFFSET + i] = 9;
		}
		VBO->write(0, 9 * INDEX_MAX, VBObuf);
		delete VBObuf;

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, 0, (GLvoid*)TEXID_OFFSET);
		glVertexAttribDivisor(0, 1);
		glVertexAttribDivisor(1, 1);
	}
	
	{/*Textures*/}
	{
		int tw = 8, th = 88, tc = 4;
		unsigned char* texData = SOIL_load_image("mines.png", &tw, &th, &tc, SOIL_LOAD_RGBA);

		glGenTextures(1, &texArray);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 8, 8, 11);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 8, 8, 11, GL_RGBA, GL_UNSIGNED_BYTE, texData);
		SOIL_free_image_data(texData);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.5019f, 0.5019f, 0.5019f, 1.0f);

		glUniform2f(dimLoc, tileWidth, tileHeight);
		
		glDrawArraysInstanced(GL_POINTS, 0, 1, INDEX_MAX);

		VBO->lock();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	delete tileState;
	return 0;
}
