#pragma once
#ifndef _LMK_ENGINE_H_
#define _LMK_ENGINE_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL
#include "LMK_systems.h"
#include "LMK_coremdl.h"
#include "LMK_render2d.h"
#include "LMK_physics2d.h"
#include "LMK_input.h"

LMK_BEGIN
class LMKEngine {
public: // Constructors & Destructors
	inline ~LMKEngine() {
#if LMK_HAVE_SDL_IMAGE
		IMG_Quit();
#endif
#if LMK_HAVE_SDL_TTF
		TTF_Quit();
#endif
#if LMK_HAVE_SDL_MIXER
		Mix_Quit();
#endif
		SDL_Quit();
	}

public: // Functions
	//
	// Initialize the LMK Engine.
	//
	inline bool Construct(std::string _title, int _x, int _y, int _w, int _h, bool _fullScreen = false) {
		m_running = false;

		// Initialize SDL
		if (!InitSDL(_title, _x, _y, _w, _h)) return false;

		// Initialize lmk::Gizmos.
		Gizmos::Init(m_renderer);

		// Initialize lmk::Screen.
		Screen::Init(m_window);

		if (_fullScreen) {
			Screen::SetFullScreenMode(FullScreenMode::Borderless);
		}

		// Initialize Lmk::Display
		Display::Init(m_renderer);

		// Initialize other properties.
		m_running = true;

		return true;
	}

	inline bool Construct(std::string _title, Vector2Int _pos, Vector2Int _size, bool _fullScreen = false) {
		return Construct(_title, _pos.x, _pos.y, _size.x, _size.y, _fullScreen);
	}

	inline bool Construct(std::string _title, RectInt _wndRect, bool _fullScreen = false) {
		return Construct(_title, _wndRect.GetPosition(), _wndRect.GetSize(), _fullScreen);
	}

	//
	// Start the constructed application.
	// 
	// This is the main loop of the application.
	//
	inline void Start() {
		OnStart();

		std::thread fixedUpdate(&LMKEngine::FixedUpdate, this);

		while (m_running) {
			EarlyUpdate();
			Update();
			LateUpdate();

			Render();
		}

		fixedUpdate.join();

		OnExit();
	}

protected: 
	virtual void OnUserStart() {}

	virtual void OnUserUpdate() {}

	virtual void OnUserFixedUpdate() {}

	virtual void OnDrawGizmos() {}

	virtual void OnUserExit() {}

private:
	//
	// Initialize the SDL subsystem and extensions.
	//
	inline bool InitSDL(std::string _title, int _x, int _y, int _w, int _h) {
		// Initialize SDL 
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			printf("Error: failed to initialize SDL subsystem.\n     >>> %s.\n", SDL_GetError());
			return false;
		}

#if LMK_HAVE_SDL_IMAGE
		// Initialize SDL_image
		auto flags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (IMG_Init(flags) != flags) {
			printf("Error: failed to initialize SDL_image.\n     >>> %s.\n", IMG_GetError());
			return false;
		}
#endif

#if LMK_HAVE_SDL_TTF
		// Initialize SDL_ttf
		if (TTF_Init() != 0) {
			printf("Error: failed to initialize SDL_ttf.\n     >>> %s.\n", TTF_GetError());
			return false;
		}
#endif

#if LMK_HAVE_SDL_MIXER
		// Initialize SDL_mixer
		flags = MIX_INIT_MP3;
		if (Mix_Init(flags) != flags) {
			printf("Error: failed to initialize SDL_mixer.\n     >>> %s.\n", Mix_GetError());
			return false;
		}
#endif

		// Initialize window
		m_window = SDL_CreateWindow(_title.c_str(), _x, _y, _w, _h, SDL_WINDOW_RESIZABLE);
		if (!m_window) {
			printf("Error: failed to initialize SDL_Window.\n");
			return false;
		}

		// Initialize targetRenderer from window
		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
		if (!m_renderer) {
			printf("Error: failed to initialize SDL_Renderer.\n");
			return false;
		}
		SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

		return true;
	}

	//
	// Handle native SDL events to update the corresponding LMK system.
	// 
	// @return
	//		Was there any new event to handle?
	//
	inline bool OnHandleEvents() {
		if (!SDL_PollEvent(&m_curEvent)) {
			m_curEvent = { };
			return false;
		}

		switch (m_curEvent.type) {
		case SDL_QUIT:
			m_running = false;
			break;
		case SDL_WINDOWEVENT:
			switch (m_curEvent.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				Screen::Update(ScreenUpdate::RESOLUTION);
				break;
			case SDL_WINDOWEVENT_MOVED:
				Screen::Update(ScreenUpdate::POSITION);
				break;
			}
			break;
		case SDL_MOUSEWHEEL:
			Input::UpdateMouseWheel(m_curEvent.wheel);
			break;
		}

		return true;
	}

	//
	// OnStart is called Get the beginning of the application, right after initialization.
	// 
	// This is only called once every run.
	//
	inline void OnStart() {
		// Handle all pending window events from initialization.
		bool cleaningUp = true;
		while (cleaningUp) {
			cleaningUp = OnHandleEvents();
		}

		Time::Update();

		OnUserStart();
	}

	//
	// FixedUpdate is called by Time.fixedUpdateTime.
	// 
	// This is used for handling physics updates.
	//
	inline void FixedUpdate() {
		while (m_running) {
			OnUserFixedUpdate();

			// Physics simulation.
			Physics2D::Step((float)Time::fixedDeltaTime());

			// Wait for Time::fixedDeltaTime before updating.
			// The smaller this value is, the more accurate the simulations will be.
			std::this_thread::sleep_for(std::chrono::duration<double> { Time::fixedDeltaTime() });
		}
	}

	//
	// EarlyUpdate is called every frame before Update().
	// 
	// This is used for updating internal systems properties of LMK Engine.
	//
	inline void EarlyUpdate() {
		Time::Update();
		Input::UpdateInputStates();
	}

	//
	// Update is called every frame.
	// 
	// All MonoBehaviour scripts Update() will be called during the execution of this function.
	//
	inline void Update() {
		OnHandleEvents();

		OnUserUpdate();
	}

	//
	// LateUpdate is called every frame after Update().
	// 
	// This is used for preserving internal systems properties of LMK Engine.
	//
	inline void LateUpdate() {
		Input::UpdateLastInputStates();
	}

	//
	// Render is called after all Update functions have been executed.
	// 
	// This is used for rendering all display information to the targetRenderer.
	//
	inline void Render() {
		Display::Clear();
		Display::Copy();
		
		OnDrawGizmos();

		Display::Render();
	}

	//
	// OnExit is called once after the last Update loop.
	// 
	// This is used for internal system cleanup.
	//
	inline void OnExit() {
		OnUserExit();

		SDL_DestroyWindow(m_window);
		SDL_DestroyRenderer(m_renderer);
	}

public: // Accessors
	// Is the main loop of LMK Engine still running?
	inline bool IsRunning() {
		return m_running;
	}

#pragma warning (disable : 26495)
public: // Properties
	std::string name;

protected:
	bool m_running;

	SDL_Window*		m_window;	// The main window.
	SDL_Renderer*	m_renderer;	// The main targetRenderer.
	SDL_Event		m_curEvent;	// An SDL structure for containing window event data.
#pragma warning (default : 26495)
};
LMK_END

#endif // LMK_HAVE_SDL
#endif // !_LMK_ENGINE_H_