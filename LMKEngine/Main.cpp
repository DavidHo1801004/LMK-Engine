#include "TestEngines.h"

int main(int argc, char* args[]) {
	ContactConstraintsTest application;

	if (!application.Construct("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1400, 800, false)) {
		std::cout << "LMKEngine: Failed to initialize engine. The program will now exit.\n";
		return EXIT_FAILURE;
	}
	
	application.Start();
	
	return EXIT_SUCCESS;
}