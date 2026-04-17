#pragma once
#ifndef _LMK_RENDER_2D_H_
#define _LMK_RENDER_2D_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL2
#include "LMK_coremdl.h"
#include "LMK_gameobj.h"
#include "LMK_delegate.h"

LMK_BEGIN
// Forward declaration.
class LMKEngine;

// +--------------------------------------------------------------------------------+
// |																				|
// | SCREEN																			|
// |																				|
// +--------------------------------------------------------------------------------+

ENUM_ENCAP_BEGIN(screen)
//
// Contains the full screen modes.
//
enum FullScreenMode {
	// Sets your application to a standard, movable window that's not full screen. 
	// 
	// The Count of the window depends on your application's resolution.
	Windowed,

	// This full screen mode is also known as 'borderless full screen'. 
	// 
	// LMK Engine renders the application Get the resolution set by a script, or the native display resolution 
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
	using target_type = SDL_Window;

	friend class LMKEngine;

public: // Constructors & Destructors
#pragma region Singleton
#pragma warning (disable : 26495)
	Screen(const Screen&) = delete;

private:
	inline Screen() = default;

#pragma warning (default : 26495)
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
		Update(ScreenUpdate::FULLSCREENMODE | ScreenUpdate::RESOLUTION);
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

	//
	// Convert LMK screen space coordinate to SDL screen space coordinate.
	// 
	// This should be called whenever LMK to SDL screen space conversion is required.
	// 
	// LMK screen space coordinate is defined with (0, 0) Get the bottom-left corner.
	// SDL screen space coordinate is defined with (0, 0) Get the top-left corner. 
	//
	[[nodiscard]] inline static Vector2 LMKToSDLSpace(Vector2 _point) {
		return Vector2{
			_point.x,
			GetHeight() - _point.y
		};
	}

private:
	//
	// Initialize the screen properties.
	// 
	// IMPORTANT:
	// This should only be called during initialization process of LMK Engine.
	// 
	// @param _target:
	//		The target SDL_Window.
	//
	inline static void Init(target_type* _target) noexcept {
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
			OnScreenResize.Invoke();
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
	// Is the main window Get full screen?
	[[nodiscard]] inline static bool IsFullScreen() {
		return (Instance.m_fullScreenMode == FullScreenMode::Borderless)
			|| (Instance.m_fullScreenMode == FullScreenMode::FullScreen);
	}

	// Get the current display mode of the application.
	[[nodiscard]] inline static FullScreenMode GetFullScreenMode() {
		return Instance.m_fullScreenMode;
	}

	// Get the current width of the screen window in pixels.
	[[nodiscard]] inline static int GetWidth() {
		return Instance.m_resolution.x;
	}

	// Get the current height of the screen window in pixels.
	[[nodiscard]] inline static int GetHeight() {
		return Instance.m_resolution.y;
	}

	// Get the current resolution of the screen window in pixels.
	[[nodiscard]] inline static Vector2Int GetResolution() {
		return Instance.m_resolution;
	}

	// Get the top left position of the main window.
	[[nodiscard]] inline static Vector2Int GetMainWindowPosition() {
		return Instance.m_mainWindowPosition;
	}

public: // Static Properties
	static Action<> OnScreenResize;

private:
	static Screen Instance;

private: // Properties
	target_type* m_target;	// The target SDL_Window.

	FullScreenMode m_fullScreenMode;	// The current display mode of the application.

	Vector2Int m_resolution;			// The current resolution in pixels of the main window.
	Vector2Int m_mainWindowPosition;	// The position of the top left corner of the main window relative to the top left corner of the display.

	// The reserved position of the main window while in FullScreenMode::Windowed.
	// This is used to avoid clipping of the main window when switching from full screen to windowed mode.
	Vector2Int m_reservedWindowPosition;
};

Screen Screen::Instance;
Action<> Screen::OnScreenResize;

// +--------------------------------------------------------------------------------+
// |																				|
// | DISPLAY																		|
// |																				|
// +--------------------------------------------------------------------------------+

// Forward declaration.
class Renderer;
class Camera2D;

//
// A static class provides access to rendering target informations.
//
class Display {

	//
	// NOTE:
	// Since we would be using SDL for most of the actual targetRenderer controls,
	// this class only acts as an additional level of abstraction for SDL_Renderer.
	// 
	// We use this class to avoid passing raw SDL_Renderer pointers everywhere.
	//

public: // Typedef
	using target_type = SDL_Renderer;
	// 
	using sorting_id_type = int32_t;

	friend class LMKEngine;
	friend Renderer;

public: // Constructors & Destructors
#pragma region Singleton
	Display(const Display&) = delete;

private:
	inline Display() = default;

public:
#pragma endregion

	~Display() = default;

private: // Static Functions
	//
	// Initialize the display properties
	// 
	// IMPORTANT:
	// This should only be called during initialization process of LMK Engine.
	// 
	// @param _target:
	//		The target SDL_Renderer.
	//
	inline static void Init(target_type* _target);

	//
	// Submit a Renderer to the display.
	// 
	// This Renderer will be displayed in the next Display::Render() call.
	// 
	// @param _renderer:
	//		A pointer to the submitting renderer.
	//
	inline static void Submit(Renderer* _renderer);

	//
	// Clear the current display using main Camera2D clear flags.
	// 
	// This should be called before any copy operation to the target SDL_Renderer.
	//
	inline static void Clear();
	
	//
	// Copy all submitted Renderers to the target SDL_Renderer.
	// 
	// This should be called before Display::Render() call.
	//
	inline static void Copy();

	//
	// Render the target SDL_Renderer to screen.
	// 
	// This should be called after all copy to the target SDL_Renderer has been made.
	//
	inline static void Render() {
		SDL_RenderPresent(Instance.m_target);
	}

	//
	// Update the ID of _renderer to _newID.
	// 
	// @param _renderer:
	//		A pointer to the _renderer to update.
	// @param _oldID:
	//		The previous ID used for sorting this _renderer.
	//
	inline static void UpdateRendererID(Renderer* _renderer, sorting_id_type _oldID);

	//
	// Remove all renderers in the display.
	//
	inline static void RemoveRenderers() {
		Instance.m_renderers.clear();
	}

public: // Accessors
	// Get a constant pointer to the Camera2D being used by the Display for rendering.
	[[nodiscard]] inline static const Camera2D* GetMainCamera() noexcept {
		return Instance.m_mainCamera;
	}

private:
	// Get a pointer to the target SDL_Renderer of the display.
	[[nodiscard]] inline static target_type* GetRenderer() {
		return Instance.m_target;
	}

public: // Mutators
	// Set the Camera2D being used by the Display for rendering.
	inline static void SetMainCamera(Camera2D* _camera) {
		Instance.m_mainCamera = _camera;
	}

private: // Static Properties
	static Display Instance;

private: // Properties
	// The target SDL_Renderer.
	// This should remain consistent through out the entire life span of the application.
	target_type* m_target = nullptr;

	// The main Camera2D of a Scene.
	// If no camera is present, the display will not be rendered.
	Camera2D* m_mainCamera;

	// A list of all submitted renderers.
	// Only Renderers that is submitted and enabled will be displayed.
	std::map<sorting_id_type, std::vector<Renderer*>> m_renderers;
};

Display Display::Instance;

// +--------------------------------------------------------------------------------+
// |																				|
// | ORTHOGRAPHIC 2D CAMERA															|
// |																				|
// +--------------------------------------------------------------------------------+

//
// A camera in LMK Engine uses the following coordinate spaces, defined as follow:
// 
//	1.	Screen space point: 
//		Defined in pixels. 
//		The bottom left of the screen is (0, 0); the top right is (Screen::width, Screen::height). 
// 
//	2.	World space point:
//		Defined in global coordinates (e.g., Transform.position).
//

#pragma warning(disable : 4244)
//
// A Camera is a device through which the player views the world.
//
class Camera2D : public Component {
public: // Constructors & Destructor
	//
	// Create a Camera 2D with the given orthographic Count.
	//
	inline Camera2D(float _orthoSize) {
		m_size = _orthoSize;
		m_pixelsPerUnit = Screen::GetHeight() / (m_size * 2);
		m_aspect = (float)Screen::GetWidth() / Screen::GetHeight();

		m_idOnScreenResize = Screen::OnScreenResize.Bind(std::bind(&Camera2D::OnScreenResize, this));

		if (m_mainCamera == nullptr) {
			m_mainCamera = this;
			Display::SetMainCamera(this);
		}
	}

	//
	// Create a default Camera 2D with orthographic Count of 5.
	//
	inline Camera2D()
		: Camera2D(5.0f) {}

	inline ~Camera2D() {
		Screen::OnScreenResize.Unbind(m_idOnScreenResize);
	}

public: // Functions
	//
	// Recalculate the aspect ratio of this camera, based on window resolution.
	//
	inline void RecalculateAspect() {
		m_aspect = (float)Screen::GetWidth() / Screen::GetHeight();
	}

	//
	// Transforms a point from screen space into world space, where world space 
	// is defined as the coordinate system at the very top of the game's hierarchy. 
	// 
	[[nodiscard]] inline Vector2 ScreenToWorldPoint(Vector2 _point) const {
		// Transform screen space to view space.
		Vector2 worldUnit{
			(_point.x / Screen::GetWidth() * m_size * m_aspect * 2) - m_size * m_aspect,
			(_point.y / Screen::GetHeight() * m_size * 2) - m_size
		};

		// Transform view space to world space.
		return m_viewMatrix.Inverse().MultiplyPoint(worldUnit);
	}

	//
	// Transforms position from world space into screen space.
	// 
	[[nodiscard]] inline Vector2 WorldToScreenPoint(Vector2 _position) const {
		// Transform world space to view space.
		_position = m_viewMatrix.MultiplyPoint(_position);

		// Transform view space to screen space.
		float screenX = ((_position.x + m_size * m_aspect) / (m_size * m_aspect * 2)) * Screen::GetWidth();
		float screenY = ((_position.y + m_size) / (m_size * 2)) * Screen::GetHeight();

		return Vector2(screenX, screenY);
	}

	//
	// Update the view matrix of this camera.
	//
	inline void RecalculateViewMatrix() {
		auto transformationMatrix = Matrix3x3::Translate(transform->GetPosition()) * Matrix3x3::Rotate(transform->GetRotation());
		m_viewMatrix = transformationMatrix.Inverse();
	}

private:
	inline void OnScreenResize() {
		RecalculateAspect();
	}

public: // Static function
	// Get the first instantiated Camera2D.
	[[nodiscard]] inline static Camera2D* GetMainCamera() noexcept {
		return m_mainCamera;
	}

public: // Accessors
	// Return the viewing volume of an orthographic Camera.
	// This is half the Count of the vertical viewing volume. 
	[[nodiscard]] inline float GetSize() const {
		return m_size;
	}

	// The aspect ratio (width divided by height).
	// By default the aspect ratio is automatically calculated from the screen's aspect ratio.
	[[nodiscard]] inline float GetAspect() const {
		return m_aspect;
	}

	// Returns pixels per world unit of this camera. This is always the result of (Screen::height() / (Count * 2))
	[[nodiscard]] inline float GetPixelsPerUnit() const {
		return m_pixelsPerUnit;
	}

public: // Mutators
	// Set the viewing volume of the camera.
	// This is half the Count of the vertical viewing volume. 
	inline void SetSize(float _size) {
		m_size = _size;
		m_pixelsPerUnit = Screen::GetHeight() / (m_size * 2);
	}

private: // Static Properties
	static Camera2D* m_mainCamera;

public: // Properties
	Color backgroundColor = Color::black;	// Color of background to clear.

private:
	// Defines the viewing volume of an orthographic Camera.
	// This is half the Count of the vertical viewing volume. 
	// The horizontal Count of the viewing volume depends on the aspect ratio.
	float m_size = 5.0f;
	float m_pixelsPerUnit = 0;	// Pixels per world unit. This is always the result of (Screen::height() / (Count * 2)) (Read Only)
	float m_aspect;				// The aspect ratio of the camera.

	Matrix3x3 m_viewMatrix;

	Action<>::action_id_type m_idOnScreenResize;
};

Camera2D* Camera2D::m_mainCamera = nullptr;
#pragma warning(default : 4244)

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
	using target_type = SDL_Surface;

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
	inline Texture2D(const Texture2D&) = default;

	inline Texture2D() = default;

	inline ~Texture2D() {
		SDL_FreeSurface(m_target);
		m_target = nullptr;
	};

public: // Operators Overloads
	[[nodiscard]] inline operator target_type*() {
		return m_target;
	}

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
	// Returns width and height of the texture.
	//
	[[nodiscard]] inline Vector2Int Size() const {
		return Vector2Int{ m_target->w, m_target->h };
	}

	//
	// Here would be where we add additional texture control functions in future implementations.
	// Right now we will keep this class as simple as an additional level of abstraction for SDL_Surface.
	//

protected: // Properties
	target_type* m_target = nullptr;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | SPRITE																			|
// |																				|
// +--------------------------------------------------------------------------------+

#pragma warning (disable : 4244)
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
	//
	//
	//
	inline explicit Sprite(Texture2D& _texture, const RectInt& _rect, Vector2 _pivot, uint32_t _pixelsPerUnit)
		: Sprite() {
		InitSprite(_texture, _rect, _pivot, _pixelsPerUnit);
	}

	inline Sprite() = default;

	inline ~Sprite() {
		delete m_bounds;
		m_bounds = nullptr;
		m_texture = nullptr;
	}

public: // Functions
	//
	// Initialize the properties of this sprite.
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
	inline void InitSprite(Texture2D& _texture, const RectInt& _rect, Vector2 _pivot, uint32_t _pixelsPerUnit) {
		SetTexture(_texture);
		m_pixelsPerUnit = _pixelsPerUnit;
		m_pivot = _pivot;
		m_textureRect = _rect;

		RecalculateBound();
	}

	inline void InitSprite(Texture2D& _texture, const RectInt& _rect) {
		InitSprite(_texture, _rect, m_pivot, m_pixelsPerUnit);
	}

	inline void InitSprite(Texture2D& _texture) {
		InitSprite(_texture, RectInt(Vector2Int::zero, _texture.Size()));
	}

private:
	inline void RecalculateBound() noexcept {
		Vector2 size = (Vector2)m_textureRect.GetSize() / m_pixelsPerUnit / 2.0f;
		m_bounds = new Bounds{
			(Vector2(0.5f, 0.5f) - m_pivot).Scale(size),
			size
		};
	}

public: // Accessors
	// Get the source Texture2D this Sprite sample from.
	[[nodiscard]] inline Texture2D& GetTexture() const noexcept {
		return *m_texture;
	}

	// Get the Bounds of the Sprite, specified by its center and extents in world space units.
	[[nodiscard]] inline Bounds GetBounds() const noexcept {
		return *m_bounds;
	}

	// Get the rectangle this Sprite uses on its Texture2D. 
	[[nodiscard]] inline RectInt GetTextureRect() const noexcept {
		return m_textureRect;
	}

	// Get the pivot point relative to its graphic rectangle. 
	[[nodiscard]] inline Vector2 GetPivot() const noexcept {
		return m_pivot;
	}

	// Get the number of pixels in the Sprite that correspond to one unit in world space.
	[[nodiscard]] inline uint32_t GetPixelsPerUnit() const noexcept {
		return m_pixelsPerUnit;
	}

public: // Mutators
	// Set the source Texture2D this Sprite sample from.
	inline void SetTexture(Texture2D& _texture) noexcept {
		m_texture = &_texture;
	}

	// Get the pivot point relative to its graphic rectangle. 
	inline void SetPivot(Vector2 _pivot) noexcept {
		m_pivot = _pivot;
	}

	// Set the number of pixels in the Sprite that correspond to one unit in world space.
	inline void SetPixelsPerUnit(uint32_t _pixelsPerUnit) noexcept {
		m_pixelsPerUnit = _pixelsPerUnit;
		RecalculateBound();
	}

private: // Properties
	Texture2D* m_texture = nullptr;	// The source Texture2D this Sprite sample from.
	
	Bounds*	m_bounds = nullptr;		// Bounds of the Sprite, specified by its center and extents in world space units.
	RectInt	m_textureRect;	// The rectangle this Sprite uses on its Texture2D in pixels.
	Vector2 m_pivot	= Vector2(0.5f, 0.5f);	// The Sprite's pivot point relative to its graphic rectangle.

	uint32_t m_pixelsPerUnit = 128;	// The number of pixels in the Sprite that correspond to one unit in world space.
};

// +--------------------------------------------------------------------------------+
// |																				|
// | RENDERER																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// General functionality for all renderers.
// 
// A targetRenderer is what makes an object appear on the screen. 
// Renderers can be disabled to make objects invisible.
//
class Renderer : public Behaviour {

	//
	// NOTE:
	// This class would be where we apply material properties to the actual output,
	// but since we do not have any implementation for shader or material yet, 
	// the rendering process will simply apply the raw texture onto the targetRenderer for now.
	//

public: // Typedef
	// An integral type. Define the maximum number of sorting layers and sorting orders.
	using sorting_id_type	= Display::sorting_id_type;
	// An integral type. This will always be half the size of sorting_id_type.
	using layer_id_type		= int16_t;

	friend class Display;

private:
	using target_type		= SDL_Texture;
	using layer_limit_type	= std::numeric_limits<layer_id_type>;
	using renderer_type		= Display::target_type;

protected: // Constructors & Destructors
	inline Renderer()
		: Behaviour() {
		m_targetRenderer = Display::GetRenderer();
		Display::Submit(this);
	}

public: // Functions
	//
	// The base function for rendering calls.
	//
	inline virtual void Render(const Camera2D& _camera) const = 0;

private:
	//
	// Get the exact sorting ID value of this Renderer.
	// 
	// Renderers with the same sortingLayerID and sortingOrder will have the same sorting ID value.
	//
	[[nodiscard]] inline sorting_id_type GetSortingID() const noexcept {
		return m_sortingLayerID * layerSize() + m_sortingOrder;
	}

private: // Static Functions
	[[nodiscard]] inline static sorting_id_type layerSize() noexcept {
		return std::abs(layer_limit_type::lowest()) + layer_limit_type::max();
	}

public: // Accessors
	// Get the unique ID of the Renderer's sorting layer. 
	[[nodiscard]] inline layer_id_type GetSortingLayerID() const noexcept {
		return m_sortingLayerID;
	}

	// Get the Renderer's order within a sorting layer.
	[[nodiscard]] inline layer_id_type GetSortingOrder() const noexcept {
		return m_sortingOrder;
	}

public: // Mutators
	// Set the unique ID of the Renderer's sorting layer. 
	inline void SetSortingLayerID(layer_id_type _layerID) noexcept {
		if (_layerID == m_sortingLayerID) return;

		auto oldID = GetSortingID();
		m_sortingLayerID = _layerID;

		Display::UpdateRendererID(this, oldID);
	}

	// Set the Renderer's order within a sorting layer.
	inline void SetSortingOrder(layer_id_type _sortingOrder) noexcept {
		if (_sortingOrder == m_sortingOrder) return; 

		auto oldID = GetSortingID();
		m_sortingOrder = _sortingOrder;

		Display::UpdateRendererID(this, oldID);
	}

protected: // Properties
	renderer_type* m_targetRenderer;
	target_type* m_texture;	// The underlying SDL_Texture used for storing SDL rendering data.

	// Unique ID of the Renderer's sorting layer. 
	// ID of the default sorting layer is always 0.
	layer_id_type m_sortingLayerID = 0;

	// Renderer's order within a sorting layer.
	// 
	// The sorting order decides what priority each GameObject has to the Renderer within each Sorting Layer.
	// The higher the number, the closer the GameObject looks to the Camera.
	// 
	// NOTE: 
	// The value must be between -32768 and 32767.
	layer_id_type m_sortingOrder = 0;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | SPRITE RENDERER																|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Renders a Sprite for 2D graphics.
//
class SpriteRenderer final : public Renderer {
public: // Functions
	inline SpriteRenderer()
		: Renderer() {}

	inline ~SpriteRenderer() {
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
	
public: // Function
	inline void Render(const Camera2D& _camera) const override {
		if (!m_isValid) return;

		SDL_RenderCopyExF(
			m_targetRenderer,				// Renderer
			m_texture,						// Texture
			m_sprite->GetTextureRect(),		// Source rect
			CalculateRenderRect(_camera),	// Dst rect
			_camera.transform->GetRotation() - transform->GetRotation(),	// Rotation
			Vector2::zero,				// Rotation pivot. This is fixed at (0, 0) since we are handling the rotation pivot using Transform.
			GetFlipDirection());			// Flip direction
	}

	[[nodiscard]] inline Rect CalculateRenderRect(const Camera2D& _camera) const {
		Vector2 worldUnitSize = (Vector2)m_sprite->GetTextureRect().GetSize() / m_sprite->GetPixelsPerUnit();
		Vector2 worldPivot = transform->TransformPoint(Vector2{
			worldUnitSize.x * (flipX ? (1 - m_sprite->GetPivot().x) : m_sprite->GetPivot().x) * -1,
			worldUnitSize.y * (flipY ? m_sprite->GetPivot().y : (1 - m_sprite->GetPivot().y))
		});
		Vector2 screenSpaceSize = Vector2::Scale((Vector2)m_sprite->GetTextureRect().GetSize() / m_sprite->GetPixelsPerUnit() * _camera.GetPixelsPerUnit(), transform->GetLossyScale());

		return Rect{
			// We update the position of the pivot to SDL coordinate.
			Screen::LMKToSDLSpace(_camera.WorldToScreenPoint(worldPivot)),
			screenSpaceSize
		};
	}

	[[nodiscard]] inline SDL_RendererFlip GetFlipDirection() const noexcept {
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		if (flipX)
			flip = SDL_FLIP_HORIZONTAL;
		if (flipY)
			flip = (flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_BOTH;
		return flip;
	}

public: // Accessors
	// Get the rendering color for the Sprite graphic.
	[[nodiscard]] inline Color GetColor() const noexcept {
		return m_tintColor;
	}

	// Get a constant reference to the Sprite attached to this targetRenderer.
	[[nodiscard]] inline const Sprite& GetSprite() const noexcept {
		return *m_sprite;
	}

public: // Mutators
	// Set the rendering color for the Sprite graphic.
	inline void SetColor(Color _color) {
		m_tintColor = _color;
		SDL_SetTextureColorMod(m_texture, m_tintColor.r, m_tintColor.g, m_tintColor.b);
		SDL_SetTextureAlphaMod(m_texture, m_tintColor.a);
	}

	// Set the a new sprite for the targetRenderer and generate a new texture from the sprite.
	inline void SetSprite(Sprite& _sprite) {
		m_isValid = true;
		m_sprite = &_sprite;
		m_texture = SDL_CreateTextureFromSurface(m_targetRenderer, m_sprite->GetTexture());

		if (m_texture == nullptr) {
			m_isValid = false;
			std::cout << "lmk::SpriteRenderer: Error during setSprite().\n     >>> " << IMG_GetError() << "\n";
		}
	}

public: // Properties
	// Rendering color for the Sprite graphic.
	// The selected vertex color becomes the rendering color, and is accessible in a pixel shader.The default color is white when no color is selected.

	// Flips the sprite on the X axis.
	// Only the rendering is affected. Use negative Transform.scale, if you want to affect all the other components (for example colliders).
	bool flipX = false;

	// Flips the sprite on the Y axis.
	// Only the rendering is affected. Use negative Transform.scale, if you want to affect all the other components (for example colliders).
	bool flipY = false;

private:
	// Rendering color for the Sprite graphic.
	// The selected vertex color becomes the rendering color, and is accessible in a pixel shader. 
	// The default color is white when no color is selected.
	Color m_tintColor = Color::white;

	Sprite* m_sprite = nullptr;	// The Sprite to render.

	bool m_isValid = false;
};

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
	Gizmos(const Gizmos&) = delete;

private:
	inline Gizmos() = default;

public:
#pragma endregion

public: // Static Functions
	//
	// Draw a line on screen.
	// 
	// @param _from:
	//		Position to start drawing the line in screen space.
	// @param _to:
	//		Position to stop drawing the in screen space.
	//
	inline static void DrawLine(Vector2 _from, Vector2 _to) {
		_from = Screen::LMKToSDLSpace(_from);
		_to = Screen::LMKToSDLSpace(_to);
		SDL_RenderDrawLineF(Instance.m_renderer, _from.x, _from.y, _to.x, _to.y);
	}

	//
	// Draw a world space line on screen.
	// 
	// @param _camera:
	//		The camera to calculate screen space coordinate from.
	// @param _from:
	//		Position to start drawing the line in world space.
	// @param _to:
	//		Position to stop drawing the in world space.
	//
	inline static void DrawLineWorld(const Camera2D& _camera, Vector2 _from, Vector2 _to) {
		_from = _camera.WorldToScreenPoint(_from);
		_to = _camera.WorldToScreenPoint(_to);
		DrawLine(_from, _to);
	}

	//
	// Draw a rectangle on screen.
	// 
	// @param _rect:
	//		The Rect to draw.
	//
	inline static void DrawRect(const Rect& _rect) {
		std::vector<Vector2> verts = {
			_rect.GetMinPos(),
			Vector2(_rect.GetXMax(), _rect.GetYMin()),
			_rect.GetMaxPos(),
			Vector2(_rect.GetXMin(), _rect.GetYMax()),
			_rect.GetMinPos()
		};
		DrawPolygon(verts);
	}

	//
	// Draw a rectangle on screen.
	// 
	// @param _center:
	//		The center of the rectangle.
	// @param _size:
	//		Half the Count of the rectangle.
	//
	inline static void DrawRect(Vector2 _center, Vector2 _extents) {
		std::vector<Vector2> verts = {
			_center - _extents,
			_center - Vector2(_extents.x, -_extents.y),
			_center + _extents,
			_center - Vector2(-_extents.x, _extents.y),
			_center - _extents,
		};
		DrawPolygon(verts);
	}

	//
	// Draw a world space rectangle on screen.
	// 
	// @param _center:
	//		The center of the rectangle.
	// @param _size:
	//		Half the Count of the rectangle.
	//
	inline static void DrawRectWorld(const Camera2D& _camera, Vector2 _center, Vector2 _extents) {
		_center = _camera.WorldToScreenPoint(_center);
		_extents *= _camera.GetPixelsPerUnit();
		DrawRect(_center, _extents);
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

	//
	// Draw a polygon defined by _vertices.
	//
	inline static void DrawPolygonWorld(const Camera2D& _camera, const std::vector<Vector2>& _vertices) {
		std::vector<Vector2> transformedVerts;
		for (auto vert : _vertices) {
			transformedVerts.push_back(_camera.WorldToScreenPoint(vert));
		}
		DrawPolygon(transformedVerts);
	}

	//
	// Draw the world grid, relative to _camera.
	//
	inline static void DrawWorldGrid(const Camera2D& _camera, float _opacity = 1.0f) {
		int grids = std::ceil(_camera.GetSize() * LMK_Max(Screen::GetHeight(), Screen::GetWidth()) / LMK_Min(Screen::GetHeight(), Screen::GetWidth()) * 1.5f);
		int rightExtent = _camera.transform->GetPosition().x + grids;
		int leftExtent = _camera.transform->GetPosition().x - grids;
		int topExtent = _camera.transform->GetPosition().y + grids;
		int bottomExtent = _camera.transform->GetPosition().y - grids;

		// Draw grid lines
		SetColor(Color::white * 0.5f * _opacity);
		for (int i = leftExtent; i <= rightExtent; i++) {
			DrawLineWorld(_camera, Vector2(i, 0) + Vector2(0, bottomExtent), Vector2(i, 0) + Vector2(0, topExtent));
		}
		for (int i = bottomExtent; i <= topExtent; i++) {
			DrawLineWorld(_camera, Vector2(0, i) + Vector2(leftExtent, 0), Vector2(0, i) + Vector2(rightExtent, 0));
		}

		// Draw original coordinate axes
		SetColor(Color::red * _opacity);
		DrawLineWorld(_camera, Vector2(leftExtent, 0), Vector2(rightExtent, 0));
		SetColor(Color::blue * _opacity);
		DrawLineWorld(_camera, Vector2(0, bottomExtent), Vector2(0, topExtent));
	}

	//
	// Draw the _camera origin.
	// 
	// @param _size:
	//		Size of the reticle to draw in pixels.
	//
	inline static void DrawCameraOrigin(float _size = 10.0f, float _opacity = 0.75f) {
		Vector2 screenCenter = Vector2{ (float)Screen::GetHeight(), (float)Screen::GetWidth() } / 2.0f;

		SetColor(Color::white * _opacity);
		DrawLine(screenCenter + Vector2{ _size, 0 }, screenCenter + Vector2{ -_size, 0 });
		DrawLine(screenCenter + Vector2{ 0, _size }, screenCenter + Vector2{ 0, -_size });
	}

private:
	//
	// Initialize the Gizmos system.
	// 
	// IMPORTANT:
	// This should only be called during initialization process of LMK Engine.
	// 
	// @param _renderer:
	//		The target SDL_Renderer for the Gizmos system to draw on.
	//
	inline static void Init(SDL_Renderer* _renderer) {
		Instance.m_renderer = _renderer;
	}

public: // Accessors
	// Get the current color used by Gizmos.
	[[nodiscard]] inline static Color GetColor() {
		return Instance.m_color;
	}

public: // Mutators
	// Set the current color used by Gizmos.
	inline static void SetColor(Color _color) {
		Instance.m_color = _color;
		SDL_SetRenderDrawColor(Instance.m_renderer, _color.r, _color.g, _color.b, _color.a);
	}

private: // Static Properties
	static Gizmos Instance;

private: // Properties
	SDL_Renderer* m_renderer = nullptr;	// The target SDL_Renderer.
	Color m_color = Color::black;		// The current display color of the Gizmos system.
};

Gizmos Gizmos::Instance;



// 0========================================================================================================0
// |																										|
// | SEPERATE IMPLEMENTATION																				|
// |																										|
// 0========================================================================================================0

// +--------------------------------------------------------------------------------+
// |																				|
// | DISPLAY																		|
// |																				|
// +--------------------------------------------------------------------------------+

inline void Display::Init(target_type* _target) {
	Instance.m_target = _target;

	// Update target SDL_Renderer of submitted renderers.
	for (auto rendererList : Instance.m_renderers)
		for (auto renderer : rendererList.second)
			renderer->m_targetRenderer = _target;
}

inline void Display::Submit(Renderer* _renderer) {
	Instance.m_renderers[_renderer->GetSortingID()].push_back(_renderer);
}

inline void Display::Clear() {
	if (!Instance.m_mainCamera) return;

	Color clear = Instance.m_mainCamera->backgroundColor;
	SDL_SetRenderDrawColor(Instance.m_target, clear.r, clear.g, clear.b, clear.a);
	SDL_RenderClear(Instance.m_target);
}

inline void Display::Copy() {
	if (!Instance.m_mainCamera) return;

	for (auto rendererList : Instance.m_renderers)
		for (auto renderer : rendererList.second)
			if (renderer->IsActiveAndEnabled()) renderer->Render(*GetMainCamera());
}

inline void Display::UpdateRendererID(Renderer* _renderer, sorting_id_type _oldID) {
	auto& rendererList = Instance.m_renderers[_oldID];
	auto iter = std::find(rendererList.begin(), rendererList.end(), _renderer);

	if (iter == rendererList.end()) return;

	rendererList.erase(iter);
	Submit(_renderer);
}

#endif // LMK_HAVE_SDL_IMAGE
LMK_END

#endif // !LMK_HAVE_SDL2

#endif // !_LMK_RENDER_2D_H_
