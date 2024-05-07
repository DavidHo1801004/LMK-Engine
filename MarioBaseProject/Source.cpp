#include "SuperMarioClone.h"

int main(int argc, char* args[])
{
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	auto width = DM.w;
	auto height = DM.h;

	SuperMarioClone game;

	if (!game.Construct("Feature Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, false)) {
		return EXIT_FAILURE;
	}
		
	game.Start();
	
	return EXIT_SUCCESS;
}