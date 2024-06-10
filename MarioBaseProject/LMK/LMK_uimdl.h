//
// \file LMK_uimdl.h
// 
// Main header for GUI elements.
// 

//
// NOTE:
// At the moment, this header only contains the essential elements to create a scalable GUI.
// Any additional classes like Button, Scrollbar, Input field, etc. might be added in future development.
//

#pragma once
#ifndef _LMK_UI_MDL_H_
#define _LMK_UI_MDL_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"

LMK_BEGIN
#if LMK_HAVE_SDL_TTF
// +--------------------------------------------------------------------------------+
// |																				|
// | TRUE TYPE FONT																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// 
//
class TTFont {
public: // Typedef
	using underlying_type = TTF_Font;

public: // Constructors & Destructors
	inline ~TTFont() {
		TTF_CloseFont(m_font);
	}

public: // Operators Overloads
	_NODISCARD inline operator TTF_Font* () const {
		return m_font;
	}

	_NODISCARD inline operator bool() {
		return m_font != nullptr;
	}

public: // Functions
	//
	// Load a new TTF or FON file to this TTFont object.
	//
	inline bool LoadFont(const char* _path, uint32_t _size) {
		m_fontPath = _path;
		m_fontSize = _size;
		m_font = TTF_OpenFont(m_fontPath, m_fontSize);
		if (!m_font) return false;
		return true;
	}

	//
	// Set a new font size.
	//
	inline bool SetFontSize(uint32_t _size) {
		m_fontSize = _size;
		m_font = TTF_OpenFont(m_fontPath, m_fontSize);
		if (!m_font) return false;
		return true;
	}

private: // Properties
	underlying_type* m_font;

	uint32_t	m_fontSize;
	const char* m_fontPath;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | GUI TEXT																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// 
//
class Text {
public:
	//
	// 
	// 
	inline void Display(SDL_Renderer* _renderer, Vector2Int _position, Color _color) {
		if (!font) return;

		SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), _color);
		if (!surface) return;

		SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
		if (!texture) return;

		SDL_Rect rect = RectInt(_position, Vector2Int(surface->w, surface->h));
		SDL_RenderCopy(_renderer, texture, NULL, &rect);

		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

public: // Properties
	std::string text;
	TTFont font;
};
#endif // c

#if LMK_HAVE_SDL2
// +--------------------------------------------------------------------------------+
// |																				|
// | CANVAS																			|
// |																				|
// +--------------------------------------------------------------------------------+

//
//
//
class Canvas {
private:
	inline Canvas(SDL_Renderer* _renderer)
		: m_renderer(_renderer) {}

private:
	SDL_Renderer* m_renderer;
};
#endif // !LMK_HAVE_SDL2
LMK_END

#endif // !_LMK_UI_MDL_H_