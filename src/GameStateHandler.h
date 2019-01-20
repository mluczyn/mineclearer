#pragma once
#include <memory>
#include "mineField.h"
#include "utils.h"

struct GameSettings
{
	unsigned int fieldWidth, fieldHeight; //Minefield dimensions
	unsigned int numMines;
};

//Tile States
enum TState
{
	covered,
	uncovered,
	flaged
};

//Game States
enum GState
{
	InProgress,
	Won,
	Lost
};

class GameStateHandler
{
public:
	GameStateHandler(GameSettings settings, std::shared_ptr<TextureUpdateHandler> textureHandler);
	void onRightClick(GLuint tile);
	void onLeftClick(GLuint tile);
	GState getGameState();
private:
	void uncoverTile(GLuint tile);
	void uncoverAdjacent(GLuint tile);
	void toggleFlag(GLuint tile);

	std::unique_ptr<Minefield> minefield;
	std::unique_ptr<TState[]> tileStates;
	std::shared_ptr<TextureUpdateHandler> textureHandler;
	GLuint numMines, tileCounter;
	GState gameState;
};

