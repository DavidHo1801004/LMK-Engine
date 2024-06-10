#include "TestEngines.h"

int main(int argc, char* args[])
{
	RenderTest application;

	if (!application.Construct("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1300, 800, false)) {
		return EXIT_FAILURE;
	}
	
	application.Start();
	
	return EXIT_SUCCESS;
}