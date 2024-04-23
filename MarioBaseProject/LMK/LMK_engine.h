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
public: // Constructors & Destructors
	inline lmkEngine(std::string _title, int _x, int _y, int _w, int _h, bool _fullscr = true) {
		if (!Init(_title, _x, _y, _w, _h, _fullscr)) {
			std::cout << "Error: failed to initialize application, the program will now exit." << std::endl;
			exit(1);
		}

		OnStart();

		while (m_running) {
			HandleEvents();
			Update();
			Render();
		}

		OnExit();
	}

	inline lmkEngine(std::string _title, Vector2Int _pos, Vector2Int _size, bool _fullscr = true)
		: lmkEngine(_title, _pos.x, _pos.y, _size.x, _size.y, _fullscr) {}

	inline lmkEngine(std::string _title, RectInt _wndRect, bool _fullscr = true)
		: lmkEngine(_title, _wndRect.position(), _wndRect.size(), _fullscr) {}

protected: // Functions
	inline void UserOnStart() {
		m_rotation = 0;
		m_moveRect = Rect(Vector2::zero(), Vector2::one() * 100);
		m_staticRect = Rect(m_screenSize / 2, Vector2::one() * 200);

		m_overlapping = false;
	}

	virtual void UserHandleEvents(const SDL_Event& _event) {
		switch (_event.type) {
		case SDL_MOUSEMOTION:
			m_mousePos.x = _event.motion.x;
			m_mousePos.y = _event.motion.y;
			break;

		case SDL_KEYDOWN:
			switch (_event.key.keysym.sym)
			{
			case SDLK_e:
				m_rotation -= 45 * m_time->DeltaTime();
				break;

			case SDLK_q:
				m_rotation += 45 * m_time->DeltaTime();
				break;

			case SDLK_w:
				m_moveRect.Scale(1 + 10 * m_time->DeltaTime());
				break;

			case SDLK_s:
				m_moveRect.Scale(1 - 10 * m_time->DeltaTime());
				break;

			case SDLK_a:
				m_staticRect.Offset(-10, 0);
				break;

			case SDLK_d:
				m_staticRect.Offset(10, 0);
				break;
			}
			break;
		}
	}

	virtual void UserUpdate() {
		m_overlapping = m_staticRect.Overlaps(m_moveRect);
		m_moveRect.setPosition(m_mousePos);
	}

	virtual void UserRender() {
		m_gizmo->setColor(Color::yellow());
		m_gizmo->DrawRect(m_staticRect);

		if (m_overlapping) {
			m_gizmo->setColor(Color::red());
		}
		else {
			m_gizmo->setColor(Color::white());
		}

		m_gizmo->DrawRect(m_moveRect, m_rotation);

	}

private:
	inline void OnStart() {
		UserOnStart();
	}

	inline void HandleEvents() {
		SDL_Event curEvent;
		SDL_PollEvent(&curEvent);

		UserHandleEvents(curEvent);

		switch (curEvent.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;
		}
	}

	inline void Update() {
		m_time->UpdateDeltaTime();

		UserUpdate();
	}

	inline void Render() {
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);

		UserRender();

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
		m_window = SDL_CreateWindow(_tilte.c_str(), _x, _y, _w, _h, flags);
		if (!m_window) {
			printf("Error: failed to initialize SDL_Window.");

			return false;
		}

		// Initialize m_renderer from window
		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
		if (!m_renderer) {
			printf("Error: failed to initialize SDL_Renderer.");

			return false;
		}

		// Initialize lmk::Gizmo subsystem
		m_gizmo = new Gizmo(m_renderer);

		// Initialize lmk::Time subsystem
		m_time = new Time();

		// Initialize protected properties
		m_screenSize = Vector2Int(_w, _h);
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
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	Gizmo* m_gizmo;
	Time* m_time;

	lmk::Vector2Int m_mousePos;
	lmk::Vector2Int m_screenSize;

private:
	bool m_running;

	bool m_overlapping;
	float m_rotation;
	Rect m_moveRect;
	Rect m_staticRect;
};
LMK_END

#endif // (LMK_HAVE_SDL) && (LMK_HAVE_SDL_IMAGE) 
#endif // !LMK_ENGINE_H_