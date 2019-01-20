#include "GameStateHandler.h"

GameStateHandler::GameStateHandler(GameSettings settings, std::shared_ptr<TextureUpdateHandler> textureHandler) : textureHandler(textureHandler), numMines(settings.numMines)
{
	minefield = std::unique_ptr<Minefield>(new Minefield(settings.fieldWidth, settings.fieldHeight));
	tileStates = std::unique_ptr<TState[]>(new TState[settings.fieldWidth * settings.fieldHeight]);
	tileCounter = (settings.fieldWidth * settings.fieldHeight) - numMines;
	gameState = Won;
}

void GameStateHandler::onRightClick(GLuint tile)
{
}

void GameStateHandler::onLeftClick(GLuint tile)
{
}

GState GameStateHandler::getGameState()
{
	return gameState;
}

void GameStateHandler::uncoverTile(GLuint tile)
{
	if (tileStates[tile] != TState::flaged)
	{
		GLubyte tileVal = minefield->getTileVal(tile);
		textureHandler->setTileTexture(tile, tileVal);
		tileStates[tile] = TState::uncovered;
		tileCounter--;
		if (!tileCounter)
		{
			gameState = Won;
		}

		if (tileVal == Texture::mine)
		{
			gameState = Lost;
			//TODO: Reveal all
		}

		//If no adjacent mines
		if (!tileVal)
		{
			auto tiles = minefield->getAdjacentTiles(tile);
			for (auto adjTile : tiles)
			{
				if (tileStates[adjTile] == TState::covered)
					uncoverTile(adjTile);
			}
		}
	}
}

void GameStateHandler::uncoverAdjacent(GLuint tile)
{
	GLubyte numAdjMines = minefield->getTileVal(tile);
	auto tiles = minefield->getAdjacentTiles(tile);
	for (auto adjTile : tiles)
		numAdjMines -= tileStates[adjTile] == TState::flaged;

	//If correct amount of tiles flagged
	if (!numAdjMines)
	{
		for (auto adjTile : tiles)
		{
			if (tileStates[adjTile] == TState::covered)
				uncoverTile(adjTile);
		}
	}
}

void GameStateHandler::toggleFlag(GLuint tile)
{
	if (tileStates[tile] == TState::covered)
	{
		textureHandler->setTileTexture(tile, Texture::flag);
		tileStates[tile] = TState::flaged;
	}
	else if (tileStates[tile] == TState::flaged)
	{
		textureHandler->setTileTexture(tile, Texture::cover);
		tileStates[tile] = TState::covered;
	}
}
