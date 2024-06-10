#pragma once
#ifndef _LMK_RENDER_2D_H_
#define _LMK_RENDER_2D_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL2
#include "LMK_coremdl.h"
#include "LMK_gameobj.h"

LMK_BEGIN
// Forward declaration.
class LMKEngine;

// +--------------------------------------------------------------------------------+
// |																				|
// | GIZMOS																			|
// |																				|
// +--------------------------------------------------------------------------------+

//
// A static class used for drawing simple shapes and lines to screen for debugging purposes.
//
class Gizmos {
public: // Typedef
	friend class LMKEngine;

public: // Constructors & Destructors
#pragma region Singleton
	inline Gizmos(const Gizmos&) = delete;

private:
	inline Gizmos() {}

public:
#pragma endregion

public: // Static Functions
	//
	// Draw a line on screen.
	// 
	// @param _from:
	//		Position to start drawing the line.
	// @param _to:
	//		Position to stop drawing the.
	//
	inline static void DrawLine(Vector2 _from, Vector2 _to) {
		SDL_RenderDrawLineF(Instance.m_renderer, _from.x, _from.y, _to.x, _to.y);
	}

	//
	// Draw a rectangle on screen.
	// 
	// @param _rect:
	//		The Rect to draw.
	//
	inline static void DrawRect(const Rect& _rect) {
		const SDL_FPoint points[5] = {
			_rect.minPos(),
			Vector2(_rect.xMax(), _rect.yMin()),
			_rect.maxPos(),
			Vector2(_rect.xMin(), _rect.yMax()),
			_rect.minPos()
		};
		SDL_RenderDrawLinesF(Instance.m_renderer, points, 5);
	}

	//
	// Draw a rectangle on screen.
	// 
	// @param _bounds:
	//		The Bounds to draw.
	//
	inline static void DrawRect(const Bounds& _bounds) {
		const SDL_FPoint points[5] = {
			_bounds.minPos(),
			Vector2(_bounds.maxPos().x, _bounds.minPos().y),
			_bounds.maxPos(),
			Vector2(_bounds.minPos().x, _bounds.maxPos().y),
			_bounds.minPos()
		};
		SDL_RenderDrawLinesF(Instance.m_renderer, points, 5);
	}

	//
	// Draw a rectangle on screen.
	// 
	// @param _center:
	//		The center of the rectangle.
	// @param _size:
	//		Half the size of the rectangle.
	//
	inline static void DrawRect(Vector2 _center, Vector2 _size) {
		const SDL_FPoint points[5] = {
			_center - _size,
			_center - Vector2(_size.x, -_size.y),
			_center + _size,
			_center - Vector2(-_size.x, _size.y),
			_center - _size,
		};
		SDL_RenderDrawLinesF(Instance.m_renderer, points, 5);
	}

	//
	// Draw a polygon defined by _vertices.
	//
	inline static void DrawPolygon(const std::vector<Vector2>& _vertices) {
		Vector2 start, end;
		for (size_t i = 0; i < _vertices.size() - 1; i++) {
			start = _vertices[i];
			end = _vertices[i + 1];
			DrawLine(start, end);
		}
		DrawLine(end, _vertices[0]);
	}

private:
	//
	// Initialize the Gizmos system.
	// 
	// @param _renderer:
	//		The target SDL_Renderer for the Gizmos system to draw on.
	//
	inline static void Init(SDL_Renderer* _renderer) {
		Instance.m_renderer = _renderer;
	}

public: // Accessors
	// Get the current color used by Gizmos.
	_NODISCARD inline static Color color() {
		return Instance.m_color;
	}

public: // Mutators
	// Set the current color used by Gizmos.
	inline static void SetColor(Color _color) {
		Instance.m_color = _color;
		SDL_SetRenderDrawColor(Instance.m_renderer, _color.r, _color.g, _color.b, _color.a);
	}

private: // Properties
	static Gizmos Instance;

	SDL_Renderer* m_renderer;		// The target SDL_Renderer.
	Color m_color = Color::black();	// The current display color of the Gizmos system.
};

Gizmos Gizmos::Instance;

// +--------------------------------------------------------------------------------+
// |																				|
// | SCREEN																			|
// |																				|
// +--------------------------------------------------------------------------------+

ENUM_ENCAP_BEGIN(screen)
//
// Cnontains the full screen modes.
//
enum FullScreenMode {
	// Sets your application to a standard, movable window that's not full screen. 
	// 
	// The size of the window depends on your application's resolution.
	Windowed,

	// This full screen mode is also known as 'borderless full screen'. 
	// 
	// LMK Engine renders the application at the resolution set by a script, or the native display resolution 
	// if no resolution is set, and scales the application resolution to fill the window. 
	// 
	// LMK Engine adds black bars ('letterboxing') to the rendered output to match the display aspect ratio, 
	// to prevent content stretching.
	Borderless,

	// Sets the application so it has sole full screen use of a display.
	// 
	// Unlike FullScreenMode.Borderless, this mode changes the operating system resolution of the display 
	// to match the application's chosen resolution.
	FullScreen,
};

//
// Contains flags to choose which properties of Screen to update.
//
enum ScreenUpdate {
	RESOLUTION		= 0x01,	// Update the resolution of the main window.
	POSITION		= 0x02,	// Update the position of the main window.
	FULLSCREENMODE	= 0x04,	// Update the full screen mode of the main window.	
	EVERYTHING		= RESOLUTION | POSITION | FULLSCREENMODE,
};
ENUM_ENCAP_END

USE_ENUM_NS(screen)

//
// A static class provides access to display information.
// 
// Use this class to get control the position and resolution of the current
// window, or hide or show the system mouse pointer.
//
class Screen {

	//
	// NOTE:
	// Since we would be using SDL for most of the actual window controls,
	// this class only acts as an additional level of abstraction for SDL_Window.
	//

public: // Typedef
	using underlying_type = SDL_Window;

	friend class LMKEngine;

public: // Constructors & Destructors
#pragma region Singleton
	inline Screen(const Screen&) = delete;

private:
	inline Screen() {}

public:
#pragma endregion

public: // Static Functions
	//
	// Switches the screen resolution.
	// 
	// Resizing the window is an asynchronous operation, which can take multiple frames.
	//
	inline static void SetMainWindowResolution(Vector2Int _resolution, FullScreenMode _fullScreenMode) noexcept {
		SDL_SetWindowSize(Instance.m_target, _resolution.x, _resolution.y);
		SetFullScreenMode(_fullScreenMode);
	}

	//
	// Set the current display mode of the application.
	//
	inline static void SetFullScreenMode(FullScreenMode _fullScreenMode) noexcept {
		switch (_fullScreenMode) {
		case FullScreenMode::Windowed:
			MoveMainWindowTo(Instance.m_reservedWindowPosition);
			SDL_SetWindowFullscreen(Instance.m_target, 0);
			break;
		case FullScreenMode::Borderless:
			Instance.m_reservedWindowPosition = Instance.m_mainWindowPosition;
			SDL_SetWindowFullscreen(Instance.m_target, SDL_WINDOW_FULLSCREEN);
			break;
		case FullScreenMode::FullScreen:
			Instance.m_reservedWindowPosition = Instance.m_mainWindowPosition;
			SDL_SetWindowFullscreen(Instance.m_target, SDL_WINDOW_FULLSCREEN_DESKTOP);
			break;
		}
		Update(ScreenUpdate::FULLSCREENMODE);
	}

	//
	// Moves the main window to the specified position relative to the top left corner of the specified display. 
	// Position value is represented in pixels. 
	// 
	// Moving the window is an asynchronous operation, which can take multiple frames.
	//
	inline static void MoveMainWindowTo(Vector2Int _position) noexcept {
		SDL_SetWindowPosition(Instance.m_target, _position.x, _position.y);
	}

private:
	//
	// Initialize the screen properties.
	// 
	// @param _target:
	//		The target SDL_Window.
	//
	inline static void Init(underlying_type* _target) noexcept {
		Instance.m_target = _target;

		Update(ScreenUpdate::EVERYTHING);
	}

	//
	// Update the screen window properties.
	// 
	// NOTE:
	// This should only be called during SDL Event handling by the LMK Engine.
	// 
	// @param _flags:
	//		The flags for the window, a mask of any of ScreenUpdate, including:
	//		::RESOLUTION,		::POSITION, 
	//		::FULLSCREENMODE,	::EVERYTHING.
	//
	inline static void Update(uint32_t _flags) noexcept {
		// Update resolution.
		if (_flags & ScreenUpdate::RESOLUTION) {
			int x, y;
			SDL_GetWindowSize(Instance.m_target, &x, &y);
			Instance.m_resolution = Vector2Int(x, y);
		}
		
		// Update position.
		if (_flags & ScreenUpdate::POSITION) {
			int x, y;
			SDL_GetWindowPosition(Instance.m_target, &x, &y);
			Instance.m_mainWindowPosition = Vector2Int(x, y);
		}

		// Update full screen mode.
		if (_flags & ScreenUpdate::FULLSCREENMODE) {
			auto flags = SDL_GetWindowFlags(Instance.m_target);
			if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
				Instance.m_fullScreenMode = FullScreenMode::FullScreen;
			}
			else if (flags & SDL_WINDOW_FULLSCREEN) {
				Instance.m_fullScreenMode = FullScreenMode::Borderless;
			}
			else {
				Instance.m_fullScreenMode = FullScreenMode::Windowed;
			}
		}
	}

public: // Accessors
	// Is the main window at full screen?
	_NODISCARD inline static bool fullScreen() {
		return (Instance.m_fullScreenMode == FullScreenMode::Borderless)
			|| (Instance.m_fullScreenMode == FullScreenMode::FullScreen);
	}

	// Get the current display mode of the application.
	_NODISCARD inline static FullScreenMode fullScreenMode() {
		return Instance.m_fullScreenMode;
	}

	// Get the current width of the screen window in pixels.
	_NODISCARD inline static int width() {
		return Instance.m_resolution.x;
	}

	// Get the current height of the screen window in pixels.
	_NODISCARD inline static int height() {
		return Instance.m_resolution.y;
	}

	// Get the current resolution of the screen window in pixels.
	_NODISCARD inline static Vector2Int resolution() {
		return Instance.m_resolution;
	}

	// Get the top left position of the main window.
	_NODISCARD inline static Vector2Int mainWindowPosition() {
		return Instance.m_mainWindowPosition;
	}

private: // Properties
	static Screen Instance;

	underlying_type* m_target;	// The target SDL_Window.

	FullScreenMode m_fullScreenMode;	// The current display mode of the application.

	Vector2Int m_resolution;			// The current resolution in pixels of the main window.
	Vector2Int m_mainWindowPosition;	// The position of the top left corner of the main window relative to the top left corner of the display.

	// The reserved position of the main window while in FullScreenMode::Windowed.
	// This is used to avoid clipping of the main window when switching from full screen to windowed mode.
	Vector2Int m_reservedWindowPosition;
};

Screen Screen::Instance;

// +--------------------------------------------------------------------------------+
// |																				|
// | DISPLAY																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// A static class provides access to rendering functions.
//
class Display {

	//
	// NOTE:
	// Since we would be using SDL for most of the actual renderer controls,
	// this class only acts as an additional level of abstraction for SDL_Renderer.
	//

public: // Typedef
	using underlying_type = SDL_Renderer;

	friend class LMKEngine;
	friend class Camera2D;

public: // Constructors & Destructors
#pragma region Singleton
	inline Display(const Display&) = delete;

private:
	inline Display() {}

public:
#pragma endregion

public: // Staitc Functions
	//
	//
	//
	inline static void Render() {

	}

	//
	// Submit a renderer to display.
	//
	inline static bool Submit() {
		return true;
	}

private:
	//
	//
	//
	inline static void Init(underlying_type* _target) {
		Instance.m_target = _target;
	}

private: // Accessors
	// Get a const pointer to the underlying type renderer of the Display.
	inline static const underlying_type* renderer() {
		return Instance.m_target;
	}

private: // Properties
	static Display Instance;

	underlying_type* m_target = nullptr;
};

Display Display::Instance;

// +--------------------------------------------------------------------------------+
// |																				|
// | RENDERER																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// 
//
class Renderer : public Component {
public:
	using underlying_type = SDL_Surface;

public: // Functions
	//
	//
	//
	inline underlying_type* GetSurface() {
		return m_target;
	}

	//
	// 
	//
	inline Rect GetRenderRect() {
		return Rect(transform->position(), Vector2(m_target->w, m_target->h));
	}

private:
	underlying_type* m_target;
};

#if LMK_HAVE_SDL_IMAGE
// +--------------------------------------------------------------------------------+
// |																				|
// | TEXTURE 2D																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Base class for Texture handling.
//
class Texture2D {
public: // Typedef
	using underlying_type = SDL_Surface;

public: // Constructors & Destructors
	//
	// Create a new Texture2D.
	// 
	// @param _path:
	//		A string contains the source image path to load from.
	//
	inline explicit Texture2D(const std::string& _path) {
		LoadRawTextureData(_path);
	};

	//
	// Copy constructor.
	//
	inline Texture2D(const Texture2D& _other) {
		m_target = new underlying_type();
		memcpy(m_target, _other.m_target, sizeof(underlying_type));
	}

	inline Texture2D() = default;

	inline ~Texture2D() {
		SDL_FreeSurface(m_target);
	};

public: // Functions
	//
	// Load texture data from an image file.
	// 
	// @return
	//		Was the loading process successful.
	//
	inline bool LoadRawTextureData(std::string _path) {
		m_target = IMG_Load(_path.c_str());
		if (m_target == nullptr) return false;
		return true;
	}

	//
	// Here would be where we add addtional texture control functions in future implementations.
	// Right now we will keep this class as simple as an additional level of abstraction for SDL_Surface.
	//

private: // Properties
	underlying_type* m_target = nullptr;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | SPRITE																			|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Represents a Sprite object for use in 2D gameplay.
// 
// Sprites are 2D graphic objects used for rendering elements of 2D gameplay. 
// The graphics are obtained from bitmap images - Texture2D. 
// The Sprite class primarily identifies the section of the image that should be used for a specific Sprite. 
// 
// This information can then be used by a SpriteRenderer component on a GameObject to actually display the graphic.
//
class Sprite final : public Object {
public: // Constructors & Destructors
	inline Sprite() = default;

private:
	//
	// Create a new Sprite from a loaded Texture2D.
	// 
	// @param _texture:
	//		The Texture2D to obtain the Sprite graphic from.
	// @param _rect:
	//		The rectangular section of the Texture to use for the Sprite.
	// @param _pivot:
	//		The Sprite's pivot point relative to its graphic rectangle.
	// @param _pixelsPerUnit:
	//		The number of pixels in the Sprite that correspond to one unit in world space.
	//
	inline explicit Sprite(Texture2D _texture, const Rect& _rect, Vector2 _pivot, uint32_t _pixelsPerUnit)
		: m_texture(_texture), m_pixelsPerUnit(_pixelsPerUnit), m_pivot(_pivot) {
		m_textureRect = new Rect(_rect);
		m_bounds = new Bounds(
			(Vector2(0.5f, 0.5f) - _pivot).Scale(_rect.size() / (float)_pixelsPerUnit / 2.0f),
			_rect.size() / (float)_pixelsPerUnit / 2.0f
		);
	}

public: // Accessors
	// Get the source Texture2D this Sprite sample from.
	_NODISCARD inline Texture2D texture() const noexcept {
		return m_texture;
	}

	// Get the Bounds of the Sprite, specified by its center and extents in world space units.
	_NODISCARD inline const Bounds* bounds() const noexcept {
		return m_bounds;
	}

	// Get the rectangle this Sprite uses on its Texture2D. 
	_NODISCARD inline const Rect* textureRect() const noexcept {
		return m_textureRect;
	}

	// Get the pivot point relative to its graphic rectangle. 
	_NODISCARD inline Vector2 pivot() const noexcept {
		return m_pivot;
	}

	// Get the number of pixels in the Sprite that correspond to one unit in world space.
	_NODISCARD inline uint32_t pixelsPerUnit() const noexcept {
		return m_pixelsPerUnit;
	}

public: // Mutators
	// Set the source Texture2D this Sprite sample from.
	inline void setTexture(Texture2D _texture) {
		m_texture = _texture;
	}

private: // Properties
	Texture2D m_texture;	// The source Texture2D this Sprite sample from.
	
	Bounds* m_bounds	= nullptr;				// Bounds of the Sprite, specified by its center and extents in world space units.
	Rect* m_textureRect	= nullptr;				// The rectangle this Sprite uses on its Texture2D.
	Vector2 m_pivot		= Vector2(0.5f, 0.5f);	// The Sprite's pivot point relative to its graphic rectangle.

	uint32_t m_pixelsPerUnit = 128;	// The number of pixels in the Sprite that correspond to one unit in world space.

};

// +--------------------------------------------------------------------------------+
// |																				|
// | SPRITE RENDERER																|
// |																				|
// +--------------------------------------------------------------------------------+

//
// 
//
class SpriteRenderer final : public Renderer {
public: // Functions

	
public: // Mutators
	//
	// 
	//
	inline void setSprite(Sprite* _sprite) {
		m_sprite = _sprite;
	}

private: // Properties
	Sprite* m_sprite;

	Bounds* m_bounds;
};
#endif // LMK_HAVE_SDL_IMAGE

// +--------------------------------------------------------------------------------+
// |																				|
// | ORTHOGRAPHIC CAMERA															|
// |																				|
// +--------------------------------------------------------------------------------+

//
// A camera in LMK Engine uses the following coordinate spaces, defined as follow:
// 
//	1.	Screen space point: 
//		Defined in pixels. 
//		The bottom left of the screen is (0, 0); the top right is (pixelWidth, pixelHeight). 
// 
//	2.	Viewport space point:
//		Normalized and relative to the Camera. 
//		The bottom left of the Camera is (0, 0); the top right is (1, 1).
// 
//	3.	World space point:
//		Defined in global coordinates (e.g., Transform.position).
//

//
// A Camera is a device through which the player views the world.
//
class Camera2D : public Component {
public: // Constructors & Destructor
	//
	// Creates a Camera 2D with the given orthographic size.
	//
	inline Camera2D(float _orthoSize)
		: size(_orthoSize) {}

	//
	// Creates a default Camera 2D.
	//
	inline Camera2D() = default;

public: // Functions
	//
	// 
	//
	inline void ResetAspect() {
		m_useDefaultAspect = true;
		m_aspect = (float)Screen::width() / Screen::height();
	}

	//
	// Transforms _position from screen space into viewport space.
	// 
	_NODISCARD inline Vector2 ScreenToViewportPoint(Vector2 _position) {

	}

	//
	// Transforms a point from screen space into world space, where world space 
	// is defined as the coordinate system at the very top of the game's hierarchy. 
	// 
	_NODISCARD inline Vector2 ScreenToWorldPoint(Vector2 _point) {

	}

	//
	// Transforms position from viewport space into screen space.
	// 
	_NODISCARD inline Vector2 ViewportToScreenPoint() {

	}

	//
	// Transforms position from viewport space into world space.
	// 
	_NODISCARD inline Vector2 ViewportToWorldPoint() {

	}

	//
	// Transforms position from world space into screen space.
	// 
	_NODISCARD inline Vector2 WorldToScreenPoint(Vector2 _position) {
		float screenX = ((_position.x + size * m_aspect) / (size * m_aspect * 2)) * Screen::width();
		float screenY = Screen::height() - ((_position.y + size) / (size * 2)) * Screen::height();
		return GetViewMatrix().MultiplyPoint(Vector2{ screenX, screenY });
	}

	//
	// Transforms position from world space into viewport space.
	// 
	_NODISCARD inline Vector2 WorldToViewportPoint() {

	}

public:
	//
	// 
	//
	_NODISCARD inline Matrix3 GetViewMatrix() {
		return Matrix3::TRS(Vector2(-transform->position().x, transform->position().y), transform->rotation(), transform->lossyScale());
	}

	//
	// 
	//
	_NODISCARD inline Matrix3 GetProjectionMatrix() {
		return Matrix3::Ortho(-size * m_aspect, size * m_aspect, -size, size);
	}

	//
	//
	//
	_NODISCARD inline Matrix3 GetProjectionViewMatrix() {
		return GetProjectionMatrix() * GetViewMatrix();
	}

public: // Accessors
	// The aspect ratio (width divided by height).
	// By default the aspect ratio is automatically calculated from the screen's aspect ratio.
	_NODISCARD inline float aspect() const {
		return m_aspect;
	}

	// Return how wide the Camera viewport is in pixels.
	_NODISCARD inline uint32_t pixelWidth() const {
		return (uint32_t)pixelRect->width();
	}

	// Return the height of the Camera viewport is in pixels.
	_NODISCARD inline uint32_t pixelHeight() const {
		return (uint32_t)pixelRect->height();
	}

public: // Mutators
	// Modify the aspect ratio of the camera, the value will stay until camera.ResetAspect() is called;
	inline void setAspect(float _aspect) {
		m_useDefaultAspect = false;
		m_aspect = _aspect;
	}

public: // Properties
	// Defines the viewing volume of an orthographic Camera.
	// This is half the size of the vertical viewing volume. 
	// The horizontal size of the viewing volume depends on the aspect ratio.
	float size	= 5.0f;	

	Color backgroundColor;

private:
	Rect* rect		= nullptr;	// Where on the screen is the camera rendered in normalized coordinates.
	Rect* pixelRect = nullptr;	// Where on the screen is the camera rendered in pixel coordinates.

	bool m_useDefaultAspect;
	float m_aspect;
};
LMK_END

#endif
#endif // !_LMK_RENDER_2D_H_
