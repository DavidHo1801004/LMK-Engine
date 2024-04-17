#include "SuperMarioClone.h"

int main(int argc, char* args[])
{
	lmk::Rect rect(0, 0, 5, 5);

	std::cout << (std::string)rect;

	rect.Offset({2, 2});

	std::cout << (std::string)rect;

	/*SuperMarioClone game(
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		500,
		500);

	while (game.isRunning()) {
		game.HandleEvents();
		game.Update();
		game.Render();
	}

	game.OnExit();*/

	return 0;
}