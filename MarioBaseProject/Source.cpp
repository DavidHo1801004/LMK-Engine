#include "SuperMarioClone.h"

int main(int argc, char* args[])
{
	SuperMarioClone game;

	if (game.Construct("Feature Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800))
		game.Start();

	return 0;
}