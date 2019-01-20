#pragma once
#include <random>
#include <chrono>
#include <vector>
#include <memory>
#include <GL\Glew.h>
#include "utils.h"

class Minefield
{
public:
	Minefield(unsigned int width, unsigned int height);
	void genField(GLuint firstTile, unsigned int numMines);
	GLubyte getTileVal(GLuint index);
	std::shared_ptr<GLuint[]> getData();
	std::vector<GLuint> getAdjacentTiles(GLuint tile);
private:

	std::unique_ptr<GLubyte[]> fieldVals;
	unsigned int fieldWidth, fieldHeight, maxIndex, mines;
};