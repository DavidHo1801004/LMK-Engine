#include "SuperMarioClone.h"

int main(int argc, char* args[])
{
	lmk::Rect rect(0, 0, 5, 5);
	lmk::Rect otherRect(1, 1, 3, 3);
	lmk::Vector2 point(3, 3);

	std::cout << (std::string)rect << std::endl;
	std::cout << (std::string)otherRect << std::endl;
	std::cout << rect.Contains(point) << std::endl;
	std::cout << rect.Overlaps(otherRect) << std::endl;

	rect.Offset(8, 8);
	std::cout << (std::string)rect << std::endl;
	std::cout << rect.Contains(point) << std::endl;
	std::cout << rect.Overlaps(otherRect) << std::endl;

	std::cout << (std::string)rect.getPosition() << std::endl;

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