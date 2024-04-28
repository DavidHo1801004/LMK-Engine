#pragma once
#ifndef _LMK_INPUT_H_
#define _LMK_INPUT_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL
LMK_BEGIN
// +--------------------------------------------------------------------------------+
// |																				|
// | INPUT SYSTEM																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
//
//
enum Keycode {
	Backspace = SDLK_BACKSPACE,
	Delete = SDLK_DELETE,
	Tab = SDLK_TAB,
	Clear = SDLK_CLEAR,
	Return = SDLK_RETURN,
	Pause = SDLK_PAUSE,
	Escape = SDLK_ESCAPE,
	Space = SDLK_SPACE,

	Keypad0 = SDLK_KP_0,
	Keypad1 = SDLK_KP_1,
	Keypad2 = SDLK_KP_2,
	Keypad3 = SDLK_KP_3,
	Keypad4 = SDLK_KP_4,
	Keypad5 = SDLK_KP_5,
	Keypad6 = SDLK_KP_6,
	Keypad7 = SDLK_KP_7,
	Keypad8 = SDLK_KP_8,
	Keypad9 = SDLK_KP_9,
	KeypadPeriod = SDLK_KP_PERIOD,
	KeypadDivide = SDLK_KP_DIVIDE,
	KeypadMultiply = SDLK_KP_MULTIPLY,
	KeypadMinus = SDLK_KP_MINUS,
	KeypadPlus = SDLK_KP_PLUS,
	KeypadEnter = SDLK_KP_ENTER,
	KeypadEquals = SDLK_KP_EQUALS,
	UpArrow = SDLK_UP,
	DownArrow = SDLK_DOWN,
	LeftArrow = SDLK_LEFT,
	RightArrow = SDLK_RIGHT,

	Insert = SDLK_INSERT,
	Home = SDLK_HOME,
	End = SDLK_END,
	PageUp = SDLK_PAGEUP,
	PageDown = SDLK_PAGEDOWN,

	F1 = SDLK_F1,
	F2 = SDLK_F2,
	F3 = SDLK_F3,
	F4 = SDLK_F4,
	F5 = SDLK_F5,
	F6 = SDLK_F6,
	F7 = SDLK_F7,
	F8 = SDLK_F8,
	F9 = SDLK_F9,
	F10 = SDLK_F10,
	F11 = SDLK_F11,
	F12 = SDLK_F12,
	F13 = SDLK_F13,
	F14 = SDLK_F14,
	F15 = SDLK_F15,

	Alpha0 = SDLK_0,
	Alpha1 = SDLK_1,
	Alpha2 = SDLK_2,
	Alpha3 = SDLK_3,
	Alpha4 = SDLK_4,
	Alpha5 = SDLK_5,
	Alpha6 = SDLK_6,
	Alpha7 = SDLK_7,
	Alpha8 = SDLK_8,
	Alpha9 = SDLK_9,

	Exclaim = SDLK_EXCLAIM,
	DoubleQuote = SDLK_QUOTEDBL,
	Hash = SDLK_HASH,
	Dollar = SDLK_DOLLAR,
	Percent = SDLK_PERCENT,
	Ampersand = SDLK_AMPERSAND,
	Quote = SDLK_QUOTE,
	LeftParen = SDLK_LEFTPAREN,
	RightParen = SDLK_RIGHTPAREN,
	Asterik = SDLK_ASTERISK,
	Plus = SDLK_PLUS,
	Comma = SDLK_COMMA,
	Minus = SDLK_MINUS,
	Period = SDLK_PERIOD,
	Slash = SDLK_SLASH,
	Colon = SDLK_COLON,
	Semicolon = SDLK_SEMICOLON,
	Less = SDLK_LESS,
	Equals = SDLK_EQUALS,
	Greater = SDLK_GREATER,
	Question = SDLK_QUESTION,
	At = SDLK_AT,
	LeftBracket = SDLK_LEFTBRACKET,
	Backslash = SDLK_BACKSLASH,
	RightBracket = SDLK_RIGHTBRACKET,
	Caret = SDLK_CARET,
	Underscore = SDLK_UNDERSCORE,
	BackQuote = SDLK_BACKQUOTE,

	A = SDLK_a,	// 'a' key.
	B = SDLK_b,	// 'b' key.
	C = SDLK_c,	// 'c' key.
	D = SDLK_d,	// 'd' key.
	E = SDLK_e,	// 'e' key.
	F = SDLK_f,	// 'f' key.
	G = SDLK_g,	// 'g' key.
	H = SDLK_h,	// 'h' key.
	I = SDLK_i,	// 'i' key.
	J = SDLK_j,	// 'j' key.
	K = SDLK_k,	// 'k' key.
	L = SDLK_l,	// 'l' key.
	M = SDLK_m,	// 'm' key.
	N = SDLK_n,	// 'n' key.
	O = SDLK_o,	// 'o' key.
	P = SDLK_p,	// 'p' key.
	Q = SDLK_q,	// 'q' key.
	R = SDLK_r,	// 'r' key.
	S = SDLK_s,	// 's' key.
	T = SDLK_t,	// 't' key.
	U = SDLK_u,	// 'u' key.
	V = SDLK_v,	// 'v' key.
	W = SDLK_w,	// 'w' key.
	X = SDLK_x,	// 'x' key.
	Y = SDLK_y,	// 'y' key.
	Z = SDLK_z,	// 'z' key.

	LeftCurlyBracket = '{',	// Left curly bracket key '{'.
	Pipe = '|',	// Pipe '|' key.
	RightCurlyBracket = '}',	// Right curly bracket key '}'.
	Tilde = '~',	// Tilde '~' key.
	/*
	Numlock			= ,	// Numlock key.
	CapsLock		= ,	// Capslock key.
	ScrollLock		= ,	// Scroll lock key.
	RightShift		= ,	// Right shift key.
	LeftShift		= ,	// Left shift key.
	RightControl	= ,	// Right Control key.
	LeftControl		= ,	// Left Control key.
	RightAlt		= ,	// Right Alt key.
	LeftAlt			= ,	// Left Alt key.
	LeftMeta		= ,	// Maps to left Windows key or left Command key if physical keys are enabled in Input Manager settings, otherwise maps to left Command key only.
	LeftCommand		= ,	// Left Command key.
	LeftApple		= ,	// Left Command key.
	LeftWindows		= ,	// Left Windows key.Deprecated if "Use Physical Keys" is enabled in Input Manager settings, use KeyCode.LeftMeta instead.
	RightMeta		= ,	// Maps to right Windows key or right Command key if physical keys are enabled in Input Manager settings, otherwise maps to right Command key only.
	RightCommand	= ,	// Right Command key.
	RightApple		= ,	// Right Command key.
	RightWindows	= ,	// Right Windows key.Deprecated if "Use Physical Keys" is enabled in Input Manager settings, use KeyCode.RightMeta instead.
	AltGr			= ,	// Alt Gr key. Deprecated if "Use Physical Keys" is enabled in Input Manager settings, use KeyCode.RightAlt instead.
	Help			= ,	// Help key.Deprecated if "Use Physical Keys" is enabled in Input Manager settings, doesn't map to any physical key.
	Print			= ,	// Print key.
	SysReq			= ,	// Sys Req key.Deprecated if "Use Physical Keys" is enabled in Input Manager settings, doesn't map to any physical key.
	Break			= ,	// Break key.Deprecated if "Use Physical Keys" is enabled in Input Manager settings, doesn't map to any physical key.
	Menu			= ,	// Menu key.

	Mouse0	// The Left(or primary) mouse button.
	Mouse1	// Right mouse button(or secondary mouse button).
	Mouse2	// Middle mouse button(or third button).
	Mouse3	// Additional(fourth) mouse button.
	Mouse4	// Additional(fifth) mouse button.
	Mouse5	// Additional(or sixth) mouse button.
	Mouse6  // Additional(or seventh) mouse button.
	*/
};

//
//
//
class Input {
public:
	//
	// 
	//
	inline static bool GetKeyDown(Keycode _key) {

		return true;
	}

	//
	// 
	//
	inline static void UpdateInputStates() {
		m_keyStates = SDL_GetKeyboardState(NULL);
		m_mouseState = SDL_GetMouseState(&m_mouseX, &m_mouseY);
	}

private:
	static const Uint8* m_keyStates;

	static Uint32 m_mouseState;
	static int m_mouseX, m_mouseY;
};

const Uint8* m_keyStates = nullptr;
Uint32 m_mouseState = 0;
LMK_END

#endif // LMK_HAVE_SDL
#endif // !_LMK_INPUT_H_
