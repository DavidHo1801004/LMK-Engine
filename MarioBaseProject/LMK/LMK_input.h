#pragma once
#ifndef _LMK_INPUT_H_
#define _LMK_INPUT_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"

#if LMK_HAVE_SDL2
LMK_BEGIN
// +--------------------------------------------------------------------------------+
// |																				|
// | INPUT ENUMS																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Key codes can be used to detect key down and key up events, 
// using Input.GetKeyDown and Input.GetKeyUp.
//
enum KeyCode {
	None			= SDL_Scancode::SDL_SCANCODE_UNKNOWN,		// Not assigned(never returned as the result of a keystroke).

	// Common command keys.
	Backspace		= SDL_Scancode::SDL_SCANCODE_BACKSPACE,		// The backspace key.
	Delete			= SDL_Scancode::SDL_SCANCODE_DELETE,		// The forward delete key.
	Tab				= SDL_Scancode::SDL_SCANCODE_TAB,			// The	tab key.
	Clear			= SDL_Scancode::SDL_SCANCODE_CLEAR,			// The Clear key.
	Return			= SDL_Scancode::SDL_SCANCODE_RETURN,		// Return key.
	Pause			= SDL_Scancode::SDL_SCANCODE_PAUSE,			// Pause on PC machines.
	Escape			= SDL_Scancode::SDL_SCANCODE_ESCAPE,		// Escape key.
	Space			= SDL_Scancode::SDL_SCANCODE_SPACE,			// Space key.

	// Keypad keys.
	Keypad0			= SDL_Scancode::SDL_SCANCODE_KP_0,			// Numeric keypad 0.
	Keypad1			= SDL_Scancode::SDL_SCANCODE_KP_1,			// Numeric keypad 1.
	Keypad2			= SDL_Scancode::SDL_SCANCODE_KP_2,			// Numeric keypad 2.
	Keypad3			= SDL_Scancode::SDL_SCANCODE_KP_3,			// Numeric keypad 3.
	Keypad4			= SDL_Scancode::SDL_SCANCODE_KP_4,			// Numeric keypad 4.
	Keypad5			= SDL_Scancode::SDL_SCANCODE_KP_5,			// Numeric keypad 5.
	Keypad6			= SDL_Scancode::SDL_SCANCODE_KP_6,			// Numeric keypad 6.
	Keypad7			= SDL_Scancode::SDL_SCANCODE_KP_7,			// Numeric keypad 7.
	Keypad8			= SDL_Scancode::SDL_SCANCODE_KP_8,			// Numeric keypad 8.
	Keypad9			= SDL_Scancode::SDL_SCANCODE_KP_9,			// Numeric keypad 9.
	KeypadPeriod	= SDL_Scancode::SDL_SCANCODE_KP_PERIOD,		// Numeric keypad '.'.
	KeypadDivide	= SDL_Scancode::SDL_SCANCODE_KP_DIVIDE,		// Numeric keypad '/'.
	KeypadMultiply	= SDL_Scancode::SDL_SCANCODE_KP_MULTIPLY,	// Numeric keypad '*'.
	KeypadMinus		= SDL_Scancode::SDL_SCANCODE_KP_MINUS,		// Numeric keypad '-'.
	KeypadPlus		= SDL_Scancode::SDL_SCANCODE_KP_PLUS,		// Numeric keypad '+'.
	KeypadEnter		= SDL_Scancode::SDL_SCANCODE_KP_ENTER,		// Numeric keypad Enter.
	KeypadEquals	= SDL_Scancode::SDL_SCANCODE_KP_EQUALS,		// Numeric keypad '='.

	// Navigation keys.
	UpArrow			= SDL_Scancode::SDL_SCANCODE_UP,			// Up arrow key.
	DownArrow		= SDL_Scancode::SDL_SCANCODE_DOWN,			// Down arrow key.
	RightArrow		= SDL_Scancode::SDL_SCANCODE_RIGHT,			// Right arrow key.
	LeftArrow		= SDL_Scancode::SDL_SCANCODE_LEFT,			// Left arrow key.
	Insert			= SDL_Scancode::SDL_SCANCODE_INSERT,		// Insert key key.
	Home			= SDL_Scancode::SDL_SCANCODE_HOME,			// Home key.
	End				= SDL_Scancode::SDL_SCANCODE_END,			// End key.
	PageUp			= SDL_Scancode::SDL_SCANCODE_PAGEUP,		// Page up.
	PageDown		= SDL_Scancode::SDL_SCANCODE_PAGEDOWN,		// Page down.

	// Function keys.
	F1				= SDL_Scancode::SDL_SCANCODE_F1,			// F1 function key.
	F2				= SDL_Scancode::SDL_SCANCODE_F2,			// F2 function key.
	F3				= SDL_Scancode::SDL_SCANCODE_F3,			// F3 function key.
	F4				= SDL_Scancode::SDL_SCANCODE_F4,			// F4 function key.
	F5				= SDL_Scancode::SDL_SCANCODE_F5,			// F5 function key.
	F6				= SDL_Scancode::SDL_SCANCODE_F6,			// F6 function key.
	F7				= SDL_Scancode::SDL_SCANCODE_F7,			// F7 function key.
	F8				= SDL_Scancode::SDL_SCANCODE_F8,			// F8 function key.
	F9				= SDL_Scancode::SDL_SCANCODE_F9,			// F9 function key.
	F10				= SDL_Scancode::SDL_SCANCODE_F10,			// F10 function key.
	F11				= SDL_Scancode::SDL_SCANCODE_F11,			// F11 function key.
	F12				= SDL_Scancode::SDL_SCANCODE_F12,			// F12 function key.
	F13				= SDL_Scancode::SDL_SCANCODE_F13,			// F13 function key.
	F14				= SDL_Scancode::SDL_SCANCODE_F14,			// F14 function key.
	F15				= SDL_Scancode::SDL_SCANCODE_F15,			// F15 function key.

	// Alphanumeric numbers.
	Alpha0			= SDL_Scancode::SDL_SCANCODE_0,				// The '0' key on the top of the alphanumeric keyboard.
	Alpha1			= SDL_Scancode::SDL_SCANCODE_1,				// The '1' key on the top of the alphanumeric keyboard.
	Alpha2			= SDL_Scancode::SDL_SCANCODE_2,				// The '2' key on the top of the alphanumeric keyboard.
	Alpha3			= SDL_Scancode::SDL_SCANCODE_3,				// The '3' key on the top of the alphanumeric keyboard.
	Alpha4			= SDL_Scancode::SDL_SCANCODE_4,				// The '4' key on the top of the alphanumeric keyboard.
	Alpha5			= SDL_Scancode::SDL_SCANCODE_5,				// The '5' key on the top of the alphanumeric keyboard.
	Alpha6			= SDL_Scancode::SDL_SCANCODE_6,				// The '6' key on the top of the alphanumeric keyboard.
	Alpha7			= SDL_Scancode::SDL_SCANCODE_7,				// The '7' key on the top of the alphanumeric keyboard.
	Alpha8			= SDL_Scancode::SDL_SCANCODE_8,				// The '8' key on the top of the alphanumeric keyboard.
	Alpha9			= SDL_Scancode::SDL_SCANCODE_9,				// The '9' key on the top of the alphanumeric keyboard.

	// Special character keys.
	Quote			= SDL_Scancode::SDL_SCANCODE_APOSTROPHE,	// Quote key '.
	Comma			= SDL_Scancode::SDL_SCANCODE_COMMA,			// Comma ',' key.
	Minus			= SDL_Scancode::SDL_SCANCODE_MINUS,			// Minus '-' key.
	Period			= SDL_Scancode::SDL_SCANCODE_PERIOD,		// Period '.' key.
	Slash			= SDL_Scancode::SDL_SCANCODE_SLASH,			// Slash '/' key.
	Semicolon		= SDL_Scancode::SDL_SCANCODE_SEMICOLON,		// Semicolon ';' key.
	Equals			= SDL_Scancode::SDL_SCANCODE_EQUALS,		// Equals '=' key.
	LeftBracket		= SDL_Scancode::SDL_SCANCODE_LEFTBRACKET,	// Left square bracket key '['.
	Backslash		= SDL_Scancode::SDL_SCANCODE_BACKSLASH,		// Backslash key '\'.
	RightBracket	= SDL_Scancode::SDL_SCANCODE_RIGHTBRACKET,	// Right square bracket key ']'.
	BackQuote		= SDL_Scancode::SDL_SCANCODE_GRAVE,			// Back quote key '`'.

	// Alphanumeric letters.
	A				= SDL_Scancode::SDL_SCANCODE_A,				// 'a' key.
	B				= SDL_Scancode::SDL_SCANCODE_B,				// 'b' key.
	C				= SDL_Scancode::SDL_SCANCODE_C,				// 'c' key.
	D				= SDL_Scancode::SDL_SCANCODE_D,				// 'd' key.
	E				= SDL_Scancode::SDL_SCANCODE_E,				// 'e' key.
	F				= SDL_Scancode::SDL_SCANCODE_F,				// 'f' key.
	G				= SDL_Scancode::SDL_SCANCODE_G,				// 'g' key.
	H				= SDL_Scancode::SDL_SCANCODE_H,				// 'h' key.
	I				= SDL_Scancode::SDL_SCANCODE_I,				// 'i' key.
	J				= SDL_Scancode::SDL_SCANCODE_J,				// 'j' key.
	K				= SDL_Scancode::SDL_SCANCODE_K,				// 'k' key.
	L				= SDL_Scancode::SDL_SCANCODE_L,				// 'l' key.
	M				= SDL_Scancode::SDL_SCANCODE_M,				// 'm' key.
	N				= SDL_Scancode::SDL_SCANCODE_N,				// 'n' key.
	O				= SDL_Scancode::SDL_SCANCODE_O,				// 'o' key.
	P				= SDL_Scancode::SDL_SCANCODE_P,				// 'p' key.
	Q				= SDL_Scancode::SDL_SCANCODE_Q,				// 'q' key.
	R				= SDL_Scancode::SDL_SCANCODE_R,				// 'r' key.
	S				= SDL_Scancode::SDL_SCANCODE_S,				// 's' key.
	T				= SDL_Scancode::SDL_SCANCODE_T,				// 't' key.
	U				= SDL_Scancode::SDL_SCANCODE_U,				// 'u' key.
	V				= SDL_Scancode::SDL_SCANCODE_V,				// 'v' key.
	W				= SDL_Scancode::SDL_SCANCODE_W,				// 'w' key.
	X				= SDL_Scancode::SDL_SCANCODE_X,				// 'x' key.
	Y				= SDL_Scancode::SDL_SCANCODE_Y,				// 'y' key.
	Z				= SDL_Scancode::SDL_SCANCODE_Z,				// 'z' key.

	// Modifier keys.
	Numlock			= SDL_Scancode::SDL_SCANCODE_NUMLOCKCLEAR,	// Numlock key.
	CapsLock		= SDL_Scancode::SDL_SCANCODE_CAPSLOCK,		// Capslock key.
	ScrollLock		= SDL_Scancode::SDL_SCANCODE_SCROLLLOCK,	// Scroll lock key.
	RightShift		= SDL_Scancode::SDL_SCANCODE_RSHIFT,		// Right shift key.
	LeftShift		= SDL_Scancode::SDL_SCANCODE_LSHIFT,		// Left shift key.
	RightControl	= SDL_Scancode::SDL_SCANCODE_RCTRL,			// Right Control key.
	LeftControl		= SDL_Scancode::SDL_SCANCODE_LCTRL,			// Left Control key.
	RightAlt		= SDL_Scancode::SDL_SCANCODE_RALT,			// Right Alt key.
	LeftAlt			= SDL_Scancode::SDL_SCANCODE_LALT,			// Left Alt key.
	LeftMeta		= SDL_Scancode::SDL_SCANCODE_LGUI,			// Maps to left Windows key or left Command (Apple) key.
	RightMeta		= SDL_Scancode::SDL_SCANCODE_RGUI,			// Maps to right Windows key or right Command (Apple) key.
	Print			= SDL_Scancode::SDL_SCANCODE_PRINTSCREEN,	// Print (a.k.a., prt sc, Print Screen) key.
	Menu			= SDL_Scancode::SDL_SCANCODE_MENU,			// Menu key.
};

//
// Mouse buttons can be used to detect mouse down and mouse up events, 
// using Input.GetMouseDownButton and Input.GetMouseButtonUp.
//
enum MouseButton {
	Left	= SDL_BUTTON_LMASK,		// Left mouse button (or primary mouse button).
	Right	= SDL_BUTTON_RMASK,		// Right mouse button (or secondary mouse button).
	Middle	= SDL_BUTTON_MMASK,		// Middle mouse button (or third mouse button).
};

// +--------------------------------------------------------------------------------+
// |																				|
// | INPUT SYSTEM																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Interface into the Input system.
// 
// KeyCode maps to physical keys only.
// 
// Note: Input flags are not reset until Update(). You should make all the Input calls in the Update() Loop.
//
class Input {
public: // Typedef
	friend class LMKEngine;

public: // Constructors & Destrutors
#pragma region Singleton
	Input(const Input&) = delete;

private:
	Input() {
		m_SDLKeyboardStates = SDL_GetKeyboardState(NULL);
		m_lastKeyboardStates = new Uint8[m_keysLength];
		memcpy(m_lastKeyboardStates, m_SDLKeyboardStates, m_keysLength);
	}

public:
#pragma endregion

	inline ~Input() {
		delete[] m_lastKeyboardStates;
		m_lastKeyboardStates = nullptr;
	}

public: // Static Functions
	//
	// Returns true while the user holds down the key identified by _key.
	//
	_NODISCARD inline static bool GetKey(KeyCode _key) {
		return Instance.m_SDLKeyboardStates[_key];
	}

	//
	// Returns true during the frame the user starts pressing down the key identified by _key.
	//
	_NODISCARD inline static bool GetKeyDown(KeyCode _key) {
		//std::cout << (int)Instance.m_lastKeyboardStates[_key] << " | " << (int)Instance.m_SDLKeyboardStates[_key] << "\n";
		return !Instance.m_lastKeyboardStates[_key] && Instance.m_SDLKeyboardStates[_key];
	}

	//
	// Returns true during the frame the user releases the key identified by _key.
	//
	_NODISCARD inline static bool GetKeyUp(KeyCode _key) {
		return Instance.m_lastKeyboardStates[_key] && !Instance.m_SDLKeyboardStates[_key];
	}

	//
	// Returns whether the given mouse button is held down.
	//
	_NODISCARD inline static bool GetMouseButton(MouseButton _button) {
		return Instance.m_mouseState & _button;
	}

	//
	// Returns true during the frame the user pressed the given mouse button.
	//
	_NODISCARD inline static bool GetMouseButtonDown(MouseButton _button) {
		return !(Instance.m_lastMouseState & _button) && (Instance.m_mouseState & _button);
	}

	//
	// Returns true during the frame the user releases the given mouse button.
	//
	_NODISCARD inline static bool GetMouseButtonUp(MouseButton _button) {
		return (Instance.m_lastMouseState & _button) && !(Instance.m_mouseState & _button);
	}

#pragma warning (disable : 4244)
	//
	// Get the current mouse position in pixel coordinates. (Read Only).
	//
	_NODISCARD inline static Vector2 GetMousePos() {
		return Vector2(Instance.m_mouseX, Instance.m_mouseY);
	}
#pragma warning (default : 4244)

private:
	//
	// Update all input flags.
	// 
	// Note: This function should only be called once within the main engine Update loop.
	//
	inline static void UpdateInputStates() {
		Instance.m_mouseState = SDL_GetMouseState(&Instance.m_mouseX, &Instance.m_mouseY);
	}

	//
	// Store the current input states for reference conditionals of next frame.
	//
	inline static void UpdateLastInputStates() {
		memcpy(Instance.m_lastKeyboardStates, Instance.m_SDLKeyboardStates, Instance.m_keysLength);
		Instance.m_lastMouseState = Instance.m_mouseState;
	}

private: // Properties
	static Input Instance;

	const uint8_t*	m_SDLKeyboardStates		= nullptr;				// Current input states since the last Update().
	uint8_t*		m_lastKeyboardStates	= nullptr;				// Previous input states for reference.
	const int		m_keysLength			= SDL_NUM_SCANCODES;	// Number of keys in the input state arrays.

	bool m_anyKey		= false;	// Is any key or mouse button currently held down? (Read Only)
	bool m_anyKeyDown	= false;	// Returns true the first frame the user hits any key or mouse button. (Read Only)

	uint32_t m_mouseState;		// Current mouse state since the last Update().
	uint32_t m_lastMouseState;	// Previous mouse state for reference.

	int m_mouseX = 0;	// Mouse position in the X axis. (Read Only)
	int m_mouseY = 0;	// Mouse position in the Y axis. (Read Only)
};

Input Input::Instance;
LMK_END

#endif // LMK_HAVE_SDL2
#endif // !_LMK_INPUT_H_
