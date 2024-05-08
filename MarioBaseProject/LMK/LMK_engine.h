#pragma once
#ifndef _LMK_ENGINE_H_
#define _LMK_ENGINE_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL
#include "LMK_systems.h"
#include "LMK_coremdl.h"
#include "LMK_gizmo.h"
#include "LMK_input.h"

LMK_BEGIN
class LMKEngine {
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

		if (_fullscr) {
			SDL_DisplayMode DM;
			SDL_GetCurrentDisplayMode(0, &DM);
			_w = DM.w;
			_h = DM.h;
		}

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
			Update();
			Render();
		}

		OnExit();
	}

protected: 
	virtual void OnUserStart() {}

	virtual void OnUserUpdate() {}

	virtual void OnDrawGizmos() {}

private:
	//
	// 
	//
	inline void OnStart() {
		Time::UpdateDeltaTime();

		OnUserStart();
	}

	//
	// Core update loop of LMK Engine.
	//
	inline void Update() {
		while (SDL_PollEvent(&m_curEvent)) {	// Loop through all occoured events ->
			if (m_curEvent.type == SDL_QUIT) {	// If the application received a Quit message ->
				m_running = false;				// Flag the application loop to terminate after this iteration.
				break;
			}
		}

		Time::UpdateDeltaTime();
		Input::UpdateInputStates();

		OnUserUpdate();

		Input::UpdateLastInputStates();
	}

	//
	// 
	//
	inline void Render() {
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);

		OnDrawGizmos();

		SDL_RenderPresent(m_renderer);
	}

	//
	// 
	//
	inline void OnExit() {
		delete m_gizmo;

		SDL_DestroyWindow(m_window);
		SDL_DestroyRenderer(m_renderer);

		IMG_Quit();
		SDL_Quit();
	}

public: // Accessors
	// Is the main loop of LMKEngine still running?
	inline bool isRunning() {
		return m_running;
	}

#pragma warning (disable : 26495)
public: // Properties
	std::string name;

protected:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	Gizmo* m_gizmo;

	Vector2Int m_screenSize;

private:
	bool m_running;

	SDL_Event m_curEvent;
#pragma warning (default : 26495)
};
LMK_END

#endif // LMK_HAVE_SDL
#endif // !_LMK_ENGINE_H_