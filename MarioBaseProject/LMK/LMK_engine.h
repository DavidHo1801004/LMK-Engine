#ifndef LMK_ENGINE_H_
#define LMK_ENGINE_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"

LMK_BEGIN
class lmkEngine {
public: // Constructors & Destructors
	inline lmkEngine(std::string _title, int _x, int _y, int _w, int _h, bool _fullscr = true) {
		if (!Init(_title, _x, _y, _w, _h, _fullscr)) {
			std::cout << "Error: failed to initialize application, the program will now exit." << std::endl;
			exit(1);
		}
	}
	inline lmkEngine(std::string _title, Vector2Int _pos, Vector2Int _size, bool _fullscr = true)
		: lmkEngine(_title, _pos.x, _pos.y, _size.x, _size.y, _fullscr) {}
	/*inline lmkEngine(std::string _title, RectInt _wndRect, bool _fullscr = true)
		: lmkEngine(_title, _wndRect.position(), _wndRect.size(), _fullscr) {}*/

	inline ~lmkEngine() {

	}

public: // Functions
	virtual void HandleEvents() {
		SDL_Event event;
		SDL_PollEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;

		default:
			break;
		}
	}

	virtual void Update() {}

	virtual void Render() {
		SDL_RenderClear(renderer);



		SDL_RenderPresent(renderer);
	}

	virtual void OnExit() {
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);

		IMG_Quit();
		SDL_Quit();
	}

private:
	inline bool Init(std::string _tilte, int _x, int _y, int _w, int _h, bool _fullscr) {
		m_running = false;

		// Initialize SDL subsystem
		int flags = 0;
		if (_fullscr) {
			flags = SDL_WINDOW_FULLSCREEN;
		}

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			printf("Error: failed to initialize SDL subsystem.\n	  >>> %s.\n", SDL_GetError());

			return false;
		}

		// Initialize window
		window = SDL_CreateWindow(_tilte.c_str(), _x, _y, _w, _h, flags);
		if (!window) {
			printf("Error: failed to initialize SDL_Window.");

			return false;
		}

		// Initialize renderer from window
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (!renderer) {
			printf("Error: failed to initialize SDL_Renderer.");

			return false;
		}

		m_running = true;

		return true;
	}

public: // Property Modifiers
	inline bool isRunning() {
		return m_running;
	}

public: // Properties
	std::string name;

protected:
	SDL_Window* window;
	SDL_Renderer* renderer;

private:
	bool m_running;

};
LMK_END

#endif // !LMK_ENGINE_H_