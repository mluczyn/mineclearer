#include "Minefield.h"

Minefield::Minefield(unsigned int width, unsigned int height) :fieldWidth(width), fieldHeight(height)
{
	fieldVals = std::unique_ptr<GLubyte[]>(new GLubyte[width * height]);
	maxIndex = (fieldWidth * fieldHeight) - 1;
}

void Minefield::genField(GLuint firstTile, unsigned int numMines)
{
	unsigned int notMines = maxIndex + 1 - numMines;
	std::minstd_rand randomGen(std::chrono::system_clock::now().time_since_epoch().count());

	//Reset tile values 
	memset(fieldVals.get(), 8, maxIndex + 1);
	for (GLuint i = 1; i < (fieldWidth - 1); ++i)
	{
		fieldVals[i] = fieldVals[fieldWidth * (fieldHeight - 1) + 1 + i] = 5;
	}
	for (GLuint i = 1; i < (fieldHeight - 1); ++i)
	{
		fieldVals[i * fieldWidth] = fieldVals[i * fieldWidth + fieldWidth - 1] = 5;
	}
	fieldVals[0] = 3;
	fieldVals[fieldWidth - 1] = 3;
	fieldVals[maxIndex] = 3;
	fieldVals[maxIndex - fieldWidth + 1] = 3;

	//0: Not considered 1: Candidate 2: Selected 
	GLubyte* tileStatus = new GLubyte[maxIndex + 1];
	memset((void*)tileStatus, 0, maxIndex + 1);
	std::vector<GLuint> candidates;

	unsigned int numAdjacent = 0;
	tileStatus[firstTile] = 2;
	notMines--;
	for (GLuint tile : getAdjacentTiles(firstTile))
	{
		numAdjacent++;
		tileStatus[tile] = 1;
		candidates.push_back(tile);
	}
	int b = 0;
	while (notMines--)
	{
		//Make sure that all tiles surrounding the first-revealed tile are marked as not mines
		unsigned int currentIndex = 0;
		if (numAdjacent)
		{
			numAdjacent--;
		}
		else
		{
			currentIndex = randomGen() % candidates.size();
		}

		GLuint currentTile = candidates[currentIndex];
		candidates.erase(candidates.begin() + currentIndex);
		b = candidates.size();
		tileStatus[currentTile] = 2;
		int x = candidates.size();
		for (GLuint tile : getAdjacentTiles(currentTile))
		{
			if (tileStatus[tile] == 0)
			{
				tileStatus[tile] = 1;
				candidates.push_back(tile);
			}
			else if (tileStatus[tile] == 2)
			{
				fieldVals[currentTile]--;
				fieldVals[tile]--;
			}
		}
	}
	delete[] tileStatus;
}
GLubyte Minefield::getTileVal(GLuint index)
{
	if (index > maxIndex)
		return 0;
	else
		return fieldVals[index];
}

std::shared_ptr<GLuint[]> Minefield::getData()
{
	return std::shared_ptr<GLuint[]>();
}

std::vector<GLuint> Minefield::getAdjacentTiles(GLuint tile)
{
	std::vector<GLuint> tiles;
	bool N = tile < (maxIndex + 1 - fieldWidth);
	bool E = (tile + 1) % fieldWidth;
	bool S = tile >= fieldWidth;
	bool W = tile % fieldWidth;
	if (N) tiles.push_back(tile + fieldWidth);
	if (N && E) tiles.push_back(tile + fieldWidth + 1);
	if (E) tiles.push_back(tile + 1);
	if (E && S) tiles.push_back(tile - fieldWidth + 1);
	if (S) tiles.push_back(tile - fieldWidth);
	if (S && W) tiles.push_back(tile - fieldWidth - 1);
	if (W) tiles.push_back(tile - 1);
	if (W && N) tiles.push_back(tile + fieldWidth - 1);
	return tiles;
}

/*
GLubyte* genMinefield(GLuint firstIndex)
{
std::minstd_rand randomGen(std::chrono::system_clock::now().time_since_epoch().count());

GLubyte* fieldMines = new GLubyte[INDEX_MAX];
memset(fieldMines, 0, INDEX_MAX);

GLuint mineIndex;
int dist;
bool N, E, S, W;
while (mines--)
{
do
{
mineIndex = randomGen() % INDEX_MAX;
//Make sure mine is not placed withing 3x3 of starting point
dist = abs((int)firstIndex - (int)mineIndex);
if (dist > 1) dist = abs(dist - fieldWidth);
} while (fieldMines[mineIndex] > 7 || mineIndex == firstIndex || dist < 2);
fieldMines[mineIndex] = 8;

N = mineIndex < (INDEX_MAX - fieldWidth);
E = (mineIndex + 1) % fieldWidth;
S = mineIndex >= fieldWidth;
W = mineIndex % fieldWidth;

if (N) fieldMines[mineIndex + fieldWidth]++;
if (N && E) fieldMines[mineIndex + fieldWidth + 1]++;
if (E) fieldMines[mineIndex + 1]++;
if (E && S) fieldMines[mineIndex - fieldWidth + 1]++;
if (S) fieldMines[mineIndex - fieldWidth]++;
if (S && W) fieldMines[mineIndex - fieldWidth - 1]++;
if (W) fieldMines[mineIndex - 1]++;
if (W && N) fieldMines[mineIndex + fieldWidth - 1]++;
}

for (int i = 0; i < INDEX_MAX; ++i)
if (fieldMines[i] > 8) fieldMines[i] = 8;

return fieldMines;
}*/