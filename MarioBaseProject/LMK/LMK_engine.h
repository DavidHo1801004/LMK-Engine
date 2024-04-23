#pragma once
#ifndef LMK_ENGINE_H_
#define LMK_ENGINE_H_

#include "LMK_stdinc.h"

#if (LMK_HAVE_SDL) && (LMK_HAVE_SDL_IMAGE)
#include "LMK_coremdl.h"
#include "LMK_gizmo.h"
#include "LMK_time.h"

LMK_BEGIN
class lmkEngine {
public: // Functions
	inline bool Construct(std::string _tilte, int _x, int _y, int _w, int _h, bool _fullscr = false) {
		m_running = false;

		// Initialize SDL 
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			printf("Error: failed to initialize SDL subsystem.\n	  >>> %s.\n", SDL_GetError());
			return false;
		}

		// Initialize window
		uint32_t flags = _fullscr ? SDL_WINDOW_FULLSCREEN : 0;

		m_window = SDL_CreateWindow(_tilte.c_str(), _x, _y, _w, _h, flags);
		if (!m_window) {
			printf("Error: failed to initialize SDL_Window.\n");
			return false;
		}

		// Initialize renderer from window
		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
		if (!m_renderer) {
			printf("Error: failed to initialize SDL_Renderer.\n");
			return false;
		}

		// Initialize lmk::Gizmo
		m_gizmo = new Gizmo(m_renderer);

		// Initialize lmk::Time
		m_time = new Time();

		// Initialize properties
		m_screenSize = Vector2Int(_w, _h);
		m_running = true;

		return true;
	}

	inline bool Construct(std::string _title, Vector2Int _pos, Vector2Int _size, bool _fullscr = false) {
		return Construct(_title, _pos.x, _pos.y, _size.x, _size.y, _fullscr);
	}

	inline bool Construct(std::string _title, RectInt _wndRect, bool _fullscr = false) {
		return Construct(_title, _wndRect.position(), _wndRect.size(), _fullscr);
	}

	inline void Start() {
		OnStart();

		while (m_running) {
			HandleEvents();
			Update();
			Render();
		}

		OnExit();
	}

protected: 
	virtual void OnUserStart() {}

	virtual void OnUserHandleEvents(const SDL_Event& _event) {}

	virtual void OnUserUpdate() {}

	virtual void OnUserRender() {}

private:
	inline void OnStart() {
		OnUserStart();
	}

	inline void HandleEvents() {
		SDL_Event curEvent;
		SDL_PollEvent(&curEvent);

		OnUserHandleEvents(curEvent);

		switch (curEvent.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;
		}
	}

	inline void Update() {
		m_time->UpdateDeltaTime();

		OnUserUpdate();
	}

	inline void Render() {
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);

		OnUserRender();

		SDL_RenderPresent(m_renderer);
	}

	inline void OnExit() {
		delete m_time;
		delete m_gizmo;

		SDL_DestroyWindow(m_window);
		SDL_DestroyRenderer(m_renderer);

		IMG_Quit();
		SDL_Quit();
	}

public: // Property Modifiers
	inline bool isRunning() {
		return m_running;
	}

public: // Properties
	std::string name;

protected:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	Gizmo* m_gizmo;
	Time* m_time;

	Vector2Int m_screenSize;

private:
	bool m_running;
};
LMK_END

#endif // (LMK_HAVE_SDL) && (LMK_HAVE_SDL_IMAGE) 
#endif // !LMK_ENGINE_H_