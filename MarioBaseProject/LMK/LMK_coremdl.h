#pragma once
#ifndef _LMK_COREMDL_H_
#define _LMK_COREMDL_H_

#include "LMK_stdinc.h"
#include "LMK_systems.h"

LMK_BEGIN
// +--------------------------------------------------------------------------------+
// |																				|
// | LAYERMASK																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Specifies Layers to use in Physics functions.
//
class LayerMask {
public: // Typedef
	using layer_size_t	= TagsLayersSystem::layer_size_t;

public:
	//
	// Create a new LayerMask with the layer id of _layers.
	// 
	// The value of _layers should not be passed manually, but instead use predefined
	// layer flags in TagsLayersSystem.
	//
	inline explicit LayerMask(uint32_t _layers)
		: m_value(_layers) {}

	//
	// Create a new LayerMask with the default id of LAYER_0.
	//
	inline LayerMask() 
		: LayerMask(TagsLayersSystem::LAYER_0) {}

public: // Functions
	//
	//
	//
	inline bool Contains(uint32_t _layerID) {
		return true;
	}

	//
	//
	//
	inline bool Contains(LayerMask _layerMask) {
		return true;
	}

public: // Static Functions
	//
	// Given a set of layer names as defined in the TagsLayersSystem, 
	// returns the equivalent layer mask for all of them.
	// 
	// @param _layerNames:
	//		List of layer names to convert to a layer mask.
	// 
	// @return:
	//		The layer mask created from the _layerNames.
	//
	inline static int GetMask(std::string _layerNames...) {
		return 0;
	}

	//
	// Given a layer number, returns the name of the layer as defined in the TagsLayersSystem.
	// 
	// @return:
	//		
	//
	inline static std::string LayerToName(layer_size_t _layer) {
		return "";
	}

	//
	// Given a layer number, returns the name of the layer as defined in the TagsLayersSystem.
	// 
	// @return:
	//		
	//
	inline static layer_size_t NameToLayer(std::string _layer) {
		return 0;
	}

public: // Accessors
	//
	// Converts a layer mask value to an integer value.
	//
	_NODISCARD inline layer_size_t value() {
		return m_value;
	}

private:
	layer_size_t m_value;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | 2D VECTOR																		|
// |																				|
// +--------------------------------------------------------------------------------+
#pragma warning(disable : 4244)
IMPL_BEGIN
//
// The base class for Vector2 and Vector2Int.
// 
// @tparam coord_type:
//		Single coordinate as an arithmetic type (e.g. int, float, ...).
//
template <typename coord_type>
class BaseVector2 {
public:	// Typedef
	using coord_t = coord_type;
	static_assert(std::is_arithmetic_v<coord_type>, "LMK Engine: Invalid coord type. (BaseVector2)");

public:	// Constructors & Destructors
	//
	// Creates a new BaseVector2<coord_type>.
	// 
	// @param _x:
	//		The X coordinate of the vector.
	// @param _y:
	//		The Y coordinate of the vector.
	//
	inline explicit BaseVector2(coord_t _x, coord_t _y)
		: x(_x), y(_y) {}

public:	// Operators Overloads
	//
	// Returns a formatted string for this vector.
	//
	// @return
	//		A formatted string: (x; y)
	//
	_NODISCARD inline operator std::string() const {
		return "(" + std::to_string(x) + "; " + std::to_string(y) + ")";
	}

	//
	// Access the x or y component using [0] or [1] respectively.
	//
	_NODISCARD constexpr coord_t& operator[](int _index) {
		switch (_index) {
		case 0:
			return x;
		case 1:
			return y;
		default:
			throw std::out_of_range("lmk::BaseVector2: index out of range for operator[] (index should be 0 or 1).");
		}
	}

	template <typename right_coord_t>
	_NODISCARD constexpr bool operator==(const BaseVector2<right_coord_t>& _right) const noexcept {
		return (x == _right.x) && (y == _right.y);
	}

public: // Functions
	//
	// The length of this vector.
	//
	// @return
	//		Square root of (x * x + y * y).
	//
	_NODISCARD constexpr float magnitude() const noexcept {
		return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
	}

	//
	// The squared length of this vector.
	//	
	// Calculating the squared magnitude instead of the magnitude is much faster.
	// Often if you are comparing magnitudes of two vectors you can just compare their squared magnitudes.
	//		
	// @return
	//		The result of (x * x + y * y).
	//
	_NODISCARD constexpr float sqrMagnitude() const noexcept {
		return std::pow(x, 2) + std::pow(y, 2);
	}

public: // Properties
	coord_t x, y;
};
IMPL_END

// 
// Representation of 2D vectors and points.
// 
class Vector2 : public impl::BaseVector2<float> {
public:	// Constructors & Destructors
	//
	// Creates a new Vector2.
	// 
	// @param _x:
	//		The X coordinate of the vector.
	// @param _y:
	//		The Y coordinate of the vector.
	//
	inline explicit Vector2(coord_t _x, coord_t _y)
		: BaseVector2(_x, _y) {}

	//
	// Creates a new Vector2 with coordinate of (0, 0).
	//
	inline explicit Vector2()
		: Vector2(0, 0) {}

public: // Operators Overloads
#if LMK_HAVE_SDL
	//
	// Converts a lmk::Vector2 to SDL_FPoint.
	//
	_NODISCARD inline operator SDL_FPoint() {
		return SDL_FPoint{ x, y };
	}
#endif

	inline Vector2& operator+=(const Vector2& _right) noexcept {
		x += _right.x;
		y += _right.y;
		return *this;
	}

	inline Vector2& operator-=(const Vector2& _right) noexcept {
		x -= _right.x;
		y -= _right.y;
		return *this;
	}

	inline Vector2& operator/=(float _right) {
		x /= _right;
		y /= _right;
		return *this;
	}

	inline Vector2& operator*=(float _right) noexcept {
		x *= _right;
		y *= _right;
		return *this;
	}

	_NODISCARD inline Vector2 operator+(const Vector2& _right) const {
		Vector2 temp = *this;
		temp += _right;
		return temp;
	}

	_NODISCARD inline Vector2 operator-(const Vector2& _right) const {
		Vector2 temp = *this;
		temp -= _right;
		return temp;
	}

	_NODISCARD inline Vector2 operator/(float _right) const {
		Vector2 temp = *this;
		temp /= _right;
		return temp;
	}

	_NODISCARD inline Vector2 operator*(float _right) const {
		Vector2 temp = *this;
		temp *= _right;
		return temp;
	}

public: // Functions
	// 
	// The normalized vector has a magnitude of 1 and is in the same direction as the current vector.
	// 
	// Note: this function does not modify the current vector. 
	// To modify and normalize the current vector, use the Normalize() function.
	// 
	// @return
	//		A normalized vector based on the current vector.
	// 
	_NODISCARD inline Vector2 normalized() const {
		return (sqrMagnitude() > 0) ? *this / magnitude() : *this;
	}

	// 
	// Makes this vector have a magnitude of 1.
	//		
	// When normalized, a vector keeps the same direction but its length is 1.0.
	// Note: This function will change the current vector. If you want to keep the current vector unchanged, use normalized().
	//		
	inline Vector2& Normalize() {
		x /= magnitude();
		y /= magnitude();
		return *this;
	}

	// 
	// Multiplies every component of this vector by the same component of _scale.
	// 
	inline Vector2& Scale(const Vector2& _scale) noexcept {
		x *= _scale.x;
		y *= _scale.y;
		return *this;
	}

	// 
	// Clamp every component of this vector to [0...1].
	// 
	inline Vector2& Clamp01() noexcept {
		x = LMK_Clamp(x, 0, 1);
		y = LMK_Clamp(y, 0, 1);
		return *this;
	}

public: // Static Functions
	// 
	// Get the direction from one Vector2 to another by using: (_to - _from).
	// 
	// @return
	//		A normalized Vector2 represents the calculated direction.
	// 
	_NODISCARD inline static Vector2 Direction(const Vector2& _from, const Vector2& _to) noexcept {
		return (_to - _from).normalized();
	}

	// 
	// Vector2.Distance(a,b) is the same as (a - b).magnitude().
	// 
	// @return
	//		The distance between _a and _b.
	// 
	_NODISCARD inline static float Distance(const Vector2& _a, const Vector2& _b) noexcept {
		return (_b - _a).magnitude();
	}

	// 
	// Clamp tthe magnitude of a Vector2.
	// 
	// @return
	//		A copy of _vector with its magnitude clamped to _maxLength.
	// 
	_NODISCARD inline static Vector2 ClampMagnitude(const Vector2& _vector, float _maxLength) {
		return _vector.normalized() * _maxLength;
	}

	// 
	// Dot Product of two vectors.
	//
	// For normalized vectors Dot returns 1 if they point in exactly the same direction;
	// -1 if they point in completely opposite directions;
	// and a number in between for other cases (e.g. Dot returns zero if vectors are perpendicular).
	//
	// For vectors of arbitrary length the Dot return values are similar:
	// they get larger when the angle between vectors decreases.
	//	
	// @return
	//		Returns (_lhs . _rhs).
	// 
	_NODISCARD inline static float Dot(const Vector2& _lhs, const Vector2& _rhs) noexcept {
		return _lhs.x * _rhs.x + _lhs.y * _rhs.y;
	}

	// 
	// Gets the unsigned angle in degrees between _from and _to.
	//
	// The angle returned is the unsigned angle between the two vectors.
	//	
	// Note: The angle returned will always be between 0 and 180 degrees, 
	// because the method returns the smallest angle between the vectors.
	// That is, it will never return a reflex angle.
	// Angles are calculated from world origin point(0, 0, 0) as the vertex.
	// 
	// @param _from:
	//		The vector from which the angular difference is measured.
	// @param _to:
	//		The vector to which the angular difference is measured.
	// 
	// @return
	//		The unsigned angle in degrees between the two vectors.
	// 
	_NODISCARD inline static float Angle(const Vector2& _from, const Vector2& _to) {
		return std::abs(SignedAngle(_from, _to));
	}

	// 
	// Gets the signed angle in degrees between _from and _to.
	//	
	// The angle returned is the signed angle between the two vectors.
	//	
	// Note: The angle returned will always be between -180 and 180 degrees,
	// because the method returns the smallest angle between the vectors.
	// That is, it will never return a reflex angle.
	// Angles are calculated from world origin point(0, 0, 0) as the vertex.
	// 
	// @param _from: 
	//		The vector from which the angular difference is measured.
	// @param _to:
	//		The vector to which the angular difference is measured.
	// 
	// @return
	//		The signed angle in degrees between the two vectors.
	// 
	_NODISCARD inline static float SignedAngle(const Vector2& _from, const Vector2& _to) {
		return LMK_RtoD(std::atan2(_from.x * _to.y - _from.y * _to.x, _from.x * _to.x + _from.y * _to.y));
	}

	// 
	// Returns a vector that is made from the largest components of two vectors.
	// 
	// @return
	//		A copy of the larger Vector2.
	// 
	_NODISCARD inline static Vector2 Max(const Vector2& _a, const Vector2& _b) noexcept {
		return (_a.sqrMagnitude() > _b.sqrMagnitude()) ? _a : _b;
	}

	// 
	// Returns a vector that is made from the smallest components of two vectors.
	// 
	// @return
	//		A copy of the smaller Vector2.
	// 
	_NODISCARD inline static Vector2 Min(const Vector2& _a, const Vector2& _b) noexcept {
		return (_a.sqrMagnitude() < _b.sqrMagnitude()) ? _a : _b;
	}

	// 
	// Linearly interpolates between vectors _a and _b by _t.
	//		
	// The parameter _t is clamped to the range [0, 1].
	//	
	// When _t = 0 returns _a
	// When _t = 1 return _b.
	// When _t = 0.5 returns the midpoint of _a and _b.
	// 
	_NODISCARD inline static Vector2 Lerp(const Vector2& _from, const Vector2& _to, float _t) noexcept {
		return LMK_Lerp(_from, _to, LMK_Clamp(_t, 0.0f, 1.0f));
	}

	// 
	// Linearly interpolates between vectors _a and _b by _t.
	// 
	// When _t = 0 returns _a.
	// When _t = 1 return _b.
	// When _t = 0.5 returns the midpoint of _a and _b.
	// 
	_NODISCARD inline static Vector2 LerpUnclamped(const Vector2& _from, const Vector2& _to, float _t) noexcept {
		return LMK_Lerp(_from, _to, _t);
	}

	// 
	// Moves a point current towards target.
	//		
	// This is essentially the same as Vector2::Lerp but instead the function will ensure 
	// that the distance never exceeds maxDistanceDelta. 
	//		
	// Negative values of maxDistanceDelta pushes the vector away from target.
	// 
	_NODISCARD inline static Vector2 MoveTowards(const Vector2& _current, const Vector2& _target, float _maxDistanceDelta) {
		if (Distance(_current, _target) <= _maxDistanceDelta)
			return _target;
		return _current + (Direction(_current, _target) * _maxDistanceDelta);
	}

	// 
	// Returns the 2D vector perpendicular to this 2D vector.
	// 
	// The result is always rotated 90-degrees in a counter-clockwise direction for a 
	// 2D coordinate system where the positive Y axis goes up.
	// 
	// @param _inDirection: 
	//		The input direction.
	// 
	// @return
	//		The perpendicular vector.
	// 
	_NODISCARD inline static Vector2 Perpendicular(const Vector2& _inDirection) {
		coord_t x = std::cos(LMK_DtoR(90)) * _inDirection.x - std::sin(LMK_DtoR(90)) * _inDirection.y;
		coord_t y = std::sin(LMK_DtoR(90)) * _inDirection.x + std::cos(LMK_DtoR(90)) * _inDirection.y;
		return Vector2(x, y);
	}

	// 
	// Reflects a vector off the surface defined by a normal
	// 
	// This method calculates a reflected vector using the following formula:
	//		v = _inDirection - _inNormal * 2 * Dot(_inDirection, _inNormal). 
	//			
	// The _inNormal vector defines a surface. A surface's normal is the vector that is 
	// perpendicular to its surface. The _inDirection vector is treated as a directional 
	// arrow coming into the surface. The returned value is a vector of equal magnitude 
	// to _inDirection but with its direction reflected.
	// 
	// @param _inDirection: 
	//		The direction vector towards the surface.
	// @param _inNormal: 
	//		The normal vector that defines the surface.
	// 
	// @return
	//		The reflected vector.
	// 
	_NODISCARD inline static Vector2 Reflect(const Vector2& _inDirection, const Vector2& _inNormal) {
		return _inDirection - (_inNormal * 2) * Dot(_inDirection, _inNormal);
	}

	// 
	// Multiplies two vectors component-wise.
	// 
	// Every component in the result is a component of _a multiplied by the same component of _b.
	// 
	_NODISCARD inline static Vector2 Scale(const Vector2& _a, const Vector2& _b) {
		coord_t x = _a.x * _b.x;
		coord_t y = _a.y * _b.y;
		return Vector2(x, y);
	}

	// 
	// Gradually changes a vector towards a desired goal over time.
	// 
	// The vector is smoothed by some spring-damper like function, which will never overshoot.
	// 
	// @param _current: 
	//		The current m_position.																					
	// @param _target: 
	//		The m_position we are trying to reach.																	
	// @param _currentVelocity: 
	//		The current velocity, this value is modified by the function every time you call it.					
	// @param _smoothTime: 
	//		Approximately the time it will take to reach the target. A smaller value will reach the target faster.
	// @param _maxSpeed: 
	//		Optionally allows you to LMK_Clamp the maximum speed.	
	//
	_NODISCARD inline static Vector2 SmoothDamp(const Vector2& _current, const Vector2& _target, Vector2& _currentVelocity,
		float _smoothTime, float _maxSpeed = INFINITY) {

		float deltaTime = 0;
		//========================= PENDING =========================//
	}

public: // Static Properties
	// 
	// Shorthand for writing Vector2(0, 1).
	// 
	_NODISCARD inline static Vector2 up() noexcept {
		return Vector2(0, 1);
	}

	// 
	// Shorthand for writing Vector2(0, -1).
	// 
	_NODISCARD inline static Vector2 down() noexcept {
		return Vector2(0, -1);
	}

	// 
	// Shorthand for writing Vector2(1, 0).
	// 
	_NODISCARD inline static Vector2 right() noexcept {
		return Vector2(1, 0);
	}

	// 
	// Shorthand for writing Vector2(-1, 0).
	// 
	_NODISCARD inline static Vector2 left() noexcept {
		return Vector2(-1, 0);
	}

	// 
	// Shorthand for writing Vector2(1, 1).
	// 
	_NODISCARD inline static Vector2 one() noexcept {
		return Vector2(1, 1);
	}

	// 
	// Shorthand for writing Vector2(0, 0).
	// 
	_NODISCARD inline static Vector2 zero() noexcept {
		return Vector2(0, 0);
	}
};

// 
// Representation of 2D vectors and points using integers.
// 
class Vector2Int : public impl::BaseVector2<int> {
public: // Constructors & Destructors
	//
	// Creates a new Vector2Int.
	// 
	// @param _x:
	//		The X coordinate of the vector.
	// @param _y:
	//		The Y coordinate of the vector.
	//
	inline explicit Vector2Int(coord_t _x, coord_t _y)
		: BaseVector2(_x, _y) {}

	//
	// Creates a new Vector2Int with coordinate of (0, 0).
	//
	inline explicit Vector2Int()
		: Vector2Int(0, 0) {}

public: // Operators Overloads
	// 
	// Converts a Vector2Int to Vector2.
	// 
	_NODISCARD inline operator Vector2() noexcept {
		return Vector2(x, y);
	}

#if LMK_HAVE_SDL
	//
	// Converts a lmk::Vector2Int to SDL_Point.
	//
	_NODISCARD inline operator SDL_Point() {
		return SDL_Point{ x, y };
	}
#endif

	inline Vector2Int& operator+=(const Vector2Int& _right) noexcept {
		x += _right.x;
		y += _right.y;
		return *this;
	}

	inline Vector2Int& operator-=(const Vector2Int& _right) noexcept {
		x -= _right.x;
		y -= _right.y;
		return *this;
	}

	inline Vector2Int& operator/=(int _right) {
		x /= _right;
		y /= _right;
		return *this;
	}

	inline Vector2Int& operator*=(int _right) noexcept {
		x *= _right;
		y *= _right;
		return *this;
	}

	_NODISCARD inline Vector2Int operator+(const Vector2Int& _right) const noexcept {
		Vector2Int temp = *this;
		temp += _right;
		return temp;
	}

	_NODISCARD inline Vector2 operator+(Vector2 _right) const noexcept {
		Vector2 temp(this->x, this->y);
		temp += _right;
		return temp;
	}

	_NODISCARD inline Vector2Int operator-(const Vector2Int& _right) const noexcept {
		Vector2Int temp = *this;
		temp -= _right;
		return temp;
	}

	_NODISCARD inline Vector2 operator-(Vector2 _right) const noexcept {
		Vector2 temp(this->x, this->y);
		temp -= _right;
		return temp;
	}

	_NODISCARD inline Vector2Int operator/(int _right) const {
		Vector2Int temp = *this;
		temp /= _right;
		return temp;
	}

	_NODISCARD inline Vector2Int operator*(int _right) const noexcept {
		Vector2Int temp = *this;
		temp *= _right;
		return temp;
	}

public: // Functions
	// 
	// Clamps the Vector2Int to the bounds given by _min and _max.
	// 
	inline Vector2Int& Clamp(const Vector2Int& _min, const Vector2Int& _max) noexcept {
		x = LMK_Clamp(x, _min.x, _max.x);
		y = LMK_Clamp(y, _min.y, _max.y);
		return *this;
	}

	// 
	// Multiplies every component of this vector by the same component of _scale.
	// 
	_NODISCARD inline void Scale(const Vector2Int& _scale) {
		x *= _scale.x;
		y *= _scale.y;
	}

public: // Static Functions
	// 
	// Shorthand for writing Vector2Int(0, 0).
	// 
	_NODISCARD inline static Vector2Int zero() noexcept {
		return Vector2Int(0, 0);
	}

	// 
	// Vector2.Distance(a,b) is the same as (a - b).magnitude().
	// 
	// @return
	//		The distance between _a and _b.
	// 
	_NODISCARD inline static float Distance(const Vector2Int& _from, const Vector2Int& _to) noexcept {
		return (_to - _from).magnitude();
	}

	// 
	// Converts a Vector2 to a Vector2Int by doing a std::ceil() to each value.
	//
	// As there is a conversion of float to integer, there is a loss of precision.
	// 
	_NODISCARD inline static Vector2Int CeilToInt(const Vector2& _v) noexcept {
		return Vector2Int(std::ceil(_v.x), std::ceil(_v.y));
	}

	// 
	// Converts a Vector2 to a Vector2Int by doing a std::floor() to each value.
	// 
	// As there is a conversion of float to integer, there is a loss of precision.
	// 
	_NODISCARD inline static Vector2Int FloorToInt(const Vector2& _v) noexcept {
		return Vector2Int(std::floor(_v.x), std::floor(_v.y));
	}

	// 
	// Converts a Vector2 to a Vector2Int by doing a std::round() to each value.
	// 
	// As there is a conversion of float to integer, there is a loss of precision.
	// 
	_NODISCARD inline static Vector2Int RoundToInt(const Vector2& _v) noexcept {
		return Vector2Int(std::round(_v.x), std::round(_v.y));
	}

	// 
	// Returns a vector that is made from the largest components of two vectors.
	// 
	// @return
	//		A copy of the larger Vector2Int.
	// 
	_NODISCARD inline static Vector2Int Max(const Vector2Int& _a, const Vector2Int& _b) noexcept {
		return (_a.sqrMagnitude() > _b.sqrMagnitude()) ? _a : _b;
	}

	// 
	// Returns a vector that is made from the smallest components of two vectors.
	// 
	// @return
	//		A copy of the smaller Vector2Int.
	// 
	_NODISCARD inline static Vector2Int Min(const Vector2Int& _a, const Vector2Int& _b) noexcept {
		return (_a.sqrMagnitude() < _b.sqrMagnitude()) ? _a : _b;
	}

	// 
	// Multiplies two vectors component-wise.
	// 
	// Every component in the result is a component of _a multiplied by the same component of _b.
	// 
	_NODISCARD inline static Vector2Int Scale(const Vector2Int& _a, const Vector2Int& _b) {
		coord_t x = _a.x * _b.x;
		coord_t y = _a.y * _b.y;
		return Vector2Int(x, y);
	}

public: // Static Properties
	// 
	// Shorthand for writing Vector2Int(0, 1).
	// 
	_NODISCARD inline static Vector2Int up() noexcept {
		return Vector2Int(0, 1);
	}

	// 
	// Shorthand for writing Vector2Int(0, -1).
	// 
	_NODISCARD inline static Vector2Int down() noexcept {
		return Vector2Int(0, -1);
	}

	// 
	// Shorthand for writing Vector2Int(1, 0).
	// 
	_NODISCARD inline static Vector2Int right() noexcept {
		return Vector2Int(1, 0);
	}

	// 
	// Shorthand for writing Vector2Int(-1, 0).
	// 
	_NODISCARD inline static Vector2Int left() noexcept {
		return Vector2Int(-1, 0);
	}

	// 
	// Shorthand for writing Vector2Int(1, 1).
	// 
	_NODISCARD inline static Vector2Int one() noexcept {
		return Vector2Int(1, 1);
	}
};

// +--------------------------------------------------------------------------------+
// |																				|
// | RECTANGLE																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Most 2D coordinate spaces in LMK Engine defines X as increasing to the right,
// and Y increasing upwards. Except for classes in the GUI space.
// 
// GUI space in LMK Engine uses a coordinate system similar to that of Window Applications, 
// where (0,0) represents the top-left corner and Y increases downwards:
// 
//				   x
//				   |	  width
//				   |---------------->
//			y ---- *----------------*
//				|  |				|
//				|  |				|
//				|  |				|
//		height	|  |				|
//				|  |				|
//				|  |				|
//				v  *----------------*
// 
// For lmk::Rect, there is an additional way to define its dimension, using the 
// X and Y coordinates of each of its edges. These are called xMin, xMax, yMin and yMax:
// 
//				 xMin			  xMax
//				   |				|
//				   |				|
//		 yMin ---- *----------------*
//				   |				|
//				   |				|
//				   |				|
//				   |				|
//				   |				|
//				   |				|
//		 yMax ---- *----------------*
//

IMPL_BEGIN
//
// Returns true when T is is any of the following:
//	> Vector2
//	> Vector2Int
//
template <typename T>
constexpr bool is_lmkvector_t = std::_Is_any_of_v<std::remove_cv_t<T>, 
	Vector2, Vector2Int>;
//
// The base class for Rect and RectInt.
// 
// @tparam coord_type:
//		Single coordinate type (e.g. int, float, ...).
// @tparam My_Vector2_type:
//		lmk::BaseVector2 type (e.g. Vector2Int, Vector2, ...).
//
template <typename coord_type, class My_Vector2_type>
class BaseRect {
protected: // Typedef
	using coord_t		= coord_type;
	static_assert(std::is_arithmetic_v<coord_type>, "LMK Engine: Invalid coord type. (BaseRect)");
	using My_Vector2_t	= typename My_Vector2_type;
	static_assert(is_lmkvector_t<My_Vector2_t>, "LMK Engine: Invalid Vector2 type. (BaseRect)");

public: // Constructors & Destructors
	//
	// Creates a BaseRect<coord_type>.
	// 
	// @param _x:
	//		The minimum X value of the rectangle.
	// @param _y:
	//		The minimum Y value of the rectangle.
	// @param _w:
	//		Width of the rectangle.
	// @param _h:
	//		Height of the rectangle.
	//
	inline explicit BaseRect(coord_t _x, coord_t _y, coord_t _w, coord_t _h)
		: m_xMin(_x), m_yMin(_y), m_xMax(_x + _w), m_yMax(_y + _h), m_width(_w), m_height(_h) {}

	//
	// Creates a BaseRect<coord_type>.
	// 
	// @param _pos:
	//		The position (x, y) of the rectangle.
	// @param _size:
	//		The width (x) and height (y) of the rectangle.
	//
	inline explicit BaseRect(const My_Vector2_t& _pos, const My_Vector2_t& _size)
		: BaseRect(_pos.x, _pos.y, _size.x, _size.y) {}

public: // Operators Overloads
	//
	// Returns a formatted string for this rectangle.
	//
	// @return
	//		A formatted string: (xMin; yMin) : (xMax; yMax)
	// 
	_NODISCARD inline operator std::string() const {
		return (std::string)My_Vector2_t(m_xMin, m_yMin) + " : " + (std::string)My_Vector2_t(m_xMax, m_yMax);
	}

	template <typename right_coord_t, class My_Vector2_t>
	_NODISCARD inline bool operator==(const BaseRect<right_coord_t, My_Vector2_t>& _right) const noexcept {
		return (m_xMin == _right.m_xMin) && (m_yMin == _right.m_yMin) && (m_width == _right.m_width) && (m_height == _right.m_height);
	}

public: // Functions
	//
	// Offset the position of the rectangle.
	//
	inline void Offset(const My_Vector2_t& _offset) noexcept {
		setPosition(position() + _offset);
	}

	//
	// Offset the position of the rectangle.
	//
	inline void Offset(coord_t _x, coord_t _y) noexcept {
		Offset(My_Vector2_t(_x, _y));
	}

	//
	// Scale width and height of the rectangle.
	//
	inline void Scale(const My_Vector2_t& _scale) noexcept {
		setWidth(m_width * _scale.x);
		setHeight(m_height * _scale.y);
	}

	//
	// Scale width and height of the rectangle.
	//
	inline void Scale(coord_t _scaleX, coord_t _scaleY) noexcept {
		Scale(My_Vector2_t(_scaleX, _scaleY));
	}

	//
	// Scale width and height of the rectangle.
	//
	inline void Scale(coord_t _scale) noexcept {
		Scale(_scale, _scale);
	}

	// 
	// Returns true if the x and y components of _point is a point inside this rectangle.
	// 
	// If _allowInverse is present and true, the width and height of the Rect are allowed 
	// to take negative values (ie, the min value is greater than the max), and the test will still work.
	// 
	// @param _point:
	//		Point to test.
	// @param _allowInverse:
	//		Does the test allow the Rect's width and height to be negative?
	// 
	// @return
	//		True if the point lies within the specified rectangle.
	// 
	_NODISCARD inline bool Contains(const My_Vector2_t& _point, bool _allowInverse = false) const noexcept {
		// If X and Y coordinate of _point falls within Min and Max range of Rect -> Contains.
		bool cond = LMK_InRange(_point.x, xMin, xMax) && LMK_InRange(_point.y, yMin, yMax);

		// If _allowInverse is true -> we also check for LMK_InRange with the other way around (ie, Min becomes Max).
		if (_allowInverse)
			return cond || (LMK_InRange(_point.x, xMax, xMin) && LMK_InRange(_point.y, yMax, yMin));
		else
			return cond;
	}

	// 
	// Returns true if the other rectangle overlaps this one.
	// 
	// @param _other:
	//		Other rectangle to test overlapping with.
	// 
	// @tparam right_coord_t: 
	//		The <coord_type> of the other rectangle.
	// 
	template <typename right_coord_t, class Right_Vector2_t>
	_NODISCARD inline bool Overlaps(const BaseRect<right_coord_t, Right_Vector2_t>& _other) const noexcept {
		// If either rectangle has area of 0 -> no overlap possible.
		if (area() == 0 || _other.area() == 0)
			return false;

		// If one rectangle is on left side of the other.
		if (m_xMin > _other.m_xMax || _other.m_xMin > m_xMax)
			return false;

		// If one rectangle is above the other.
		if (m_yMin > _other.m_yMax || _other.m_yMin > m_yMax)
			return false;

		return true;
	}

protected:
	// Get the area of this Rect.
	_NODISCARD inline coord_t area() const noexcept {
		return m_width * m_height;
		return m_width * m_height;
	}

public: // Accessors
	// Returns the width of the rectangle, measured from the X position.
	_NODISCARD inline coord_t width() const noexcept {
		return m_width;
	}

	// Returns the height of the rectangle, measured from the Y position.
	_NODISCARD inline coord_t height() const noexcept {
		return m_height;
	}

	// Get the width and height of the rectangle.
	_NODISCARD inline My_Vector2_t size() const noexcept {
		return My_Vector2_t(m_width, m_height);
	}

	// Get the minimum X coordinate of the rectangle. 
	_NODISCARD inline coord_t xMin() const noexcept {
		return m_xMin;
	}

	// Get the minimum Y coordinate of the rectangle. 
	_NODISCARD inline coord_t yMin() const noexcept {
		return m_yMin;
	}

	// Get the position of the minimum corner of the rectangle.
	_NODISCARD inline My_Vector2_t minPos() const noexcept {
		return My_Vector2_t(m_xMin, m_yMin);
	}

	// Get the maximum X coordinate of the rectangle. 
	_NODISCARD inline coord_t xMax() const noexcept {
		return m_xMax;
	}

	// Get the maximum Y coordinate of the rectangle. 
	_NODISCARD inline coord_t yMax() const noexcept {
		return m_yMax;
	}

	// Get the position of the maximum corner of the rectangle.
	_NODISCARD inline My_Vector2_t maxPos() const noexcept {
		return My_Vector2_t(m_xMax, m_yMax);
	}

	// Get the X and Y position of the rectangle.
	_NODISCARD inline My_Vector2_t position() const noexcept {
		return My_Vector2_t(m_xMin, m_yMin);
	}

public: // Mutators
	// Set the width of the rectangle, measured from the X position.
	// Setting this property will preserve the Min coordinate but changes and Max coordinate.
	inline void setWidth(coord_t _width) noexcept {
		m_width = _width;
		m_xMax = m_xMin + m_width;
	}

	// Set the height of the rectangle, measured from the Y position.
	// Setting this property will preserve the Min coordinate but changes and Max coordinate.
	inline void setHeight(coord_t _height) noexcept {
		m_height = _height;
		m_yMax = m_yMin + m_height;
	}

	// Set the width and height of the rectangle.
	// Setting this property will preserve the Min coordinate but changes and Max coordinate.
	inline void setSize(coord_t _width, coord_t _height) noexcept {
		m_width = _width;
		m_height = _height;
		m_xMax = m_xMin + m_width;
		m_yMax = m_yMin + m_height;
	}

	// Set the width and height of the rectangle.
	// Setting this property will preserve the Min coordinate but changes and Max coordinate.
	inline void setSize(const My_Vector2_t& _size) noexcept {
		setSize(_size.x, _size.y);
	}

	// Set the minimum X coordinate of the rectangle. 
	// Setting this property will resize the width of the rectangle.
	inline void setXMin(coord_t _xMin) noexcept {
		m_xMin = _xMin;
		m_width = m_xMax - m_xMin;
	}

	// Set the minimum X coordinate of the rectangle. 
	// Setting this property will resize the height of the rectangle.
	inline void setYMin(coord_t _yMin) noexcept {
		m_yMin = _yMin;
		m_height = m_yMax - m_yMin;
	}

	// Set the position of the minimum corner of the rectangle.
	// Setting this property will resize the rectangle and preserve the position of the Max coordinate.
	inline void setMin(coord_t _x, coord_t _y) noexcept {
		m_xMin = _x;
		m_yMin = _y;
		m_width = m_xMax - m_xMin;
		m_height = m_yMax - m_yMin;
	}

	// Set the position of the minimum corner of the rectangle.
	// Setting this property will resize the rectangle and preserve the position of the Max coordinate.
	inline void setMin(const My_Vector2_t& _minPos) noexcept {
		setMin(_minPos.x, _minPos.y);
	}

	// Set the maximum X coordinate of the rectangle. 
	// Setting this property will resize the width of the rectangle.
	inline void setXMax(coord_t _xMax) noexcept {
		m_xMax = _xMax;
		m_width = m_xMax - m_xMin;
	}

	// Set the maximum Y coordinate of the rectangle. 
	// Setting this property will resize the height of the rectangle.
	inline void setYMax(coord_t _yMax) noexcept {
		m_yMax = _yMax;
		m_height = m_yMax - m_yMin;
	}

	// Set the position of the maximum corner of the rectangle.
	// Setting this property will resize the rectangle and preserve the position of the Min coordinate.
	inline void setMax(coord_t _x, coord_t _y) noexcept {
		m_xMax = _x;
		m_yMax = _y;
		m_width = m_xMax - m_xMin;
		m_height = m_yMax - m_yMin;
	}

	// Set the position of the maximum corner of the rectangle.
	// Setting this property will resize the rectangle and preserve the position of the Min coordinate.
	inline void setMax(const My_Vector2_t& _maxPos) noexcept {
		setMax(_maxPos.x, _maxPos.y);
	}

	// Set the X and Y position of the rectangle. 
	// Setting this property will preserve the size of the rectangle but changes the Min and Max coordinate.
	inline void setPosition(coord_t _x, coord_t _y) noexcept {
		m_xMin = _x;
		m_yMin = _y;
		m_xMax = _x + m_width;
		m_yMax = _y + m_height;
	}

	// Set the X and Y position of the rectangle.
	// Setting this property will preserve the size of the rectangle but changes the Min and Max coordinate.
	inline void setPosition(const My_Vector2_t& _pos) noexcept {
		setPosition(_pos.x, _pos.y);
	}

public: // Properties
	//
	// The vertial center of the rectangle.
	// This property acts as a pivot for Rect-based components to perform scale and rotation functions.
	//
	Vector2 center = Vector2(0.5f, 0.5f);

protected:
	// The position of the minimum corner of the rectangle.
	coord_t m_xMin, m_yMin;
	// The position of the maximum corner of the rectangle.
	coord_t m_xMax, m_yMax;

	coord_t m_width, m_height;
};
IMPL_END

//
// A 2D Rectangle defined by X and Y m_position, width and height.
// 
class Rect : public impl::BaseRect<float, Vector2> {
public:	// Constructors & Destructors
	//
	// Creates a new Rect.
	// 
	// @param _x:
	//		The minimum X value of the rectangle.
	// @param _y:
	//		The minimum Y value of the rectangle.
	// @param _w:
	//		Width of the rectangle.
	// @param _h:
	//		Height of the rectangle.
	//
	inline explicit Rect(coord_t _x, coord_t _y, coord_t _w, coord_t _h)
		: BaseRect(_x, _y, _w, _h) {}

	//
	// Creates a new Rect.
	// 
	// @param _pos:
	//		The position (x, y) of the rectangle.
	// @param _size:
	//		The width (x) and height (y) of the rectangle.
	//
	inline explicit Rect(const My_Vector2_t& _position, const My_Vector2_t& _size)
		: Rect(_position.x, _position.y, _size.x, _size.y) {}

	//
	// Creates a new Rect with position and size of (0, 0).
	//
	inline Rect() 
		: Rect(0, 0, 0, 0) {}

public: // Operators Overloads
#if LMK_HAVE_SDL
	//
	// Convert a lmk::RectInt to SDL_Rect.
	//
	_NODISCARD inline operator SDL_FRect() {
		return SDL_FRect{ m_xMin, m_yMin, m_width, m_height };
	}
#endif

public: // Static Functions
	//
	// Get a point inside a rectangle, given normalized coordinates.
	// 
	// @param _rect:
	//		The rectangle to get a point inside.
	// @param _normRectCoord:
	//		Normalized coordinates to get a point for.
	//
	_NODISCARD inline static My_Vector2_t NormalizedToPoint(const Rect& _rect, const My_Vector2_t& _normRectCoord) {
		return My_Vector2_t::Scale(_normRectCoord, _rect.size());
	}

	//
	// Get the normalized coordinates cooresponding to the given point.
	// 
	// @return:
	//		A Vector2 in the range 0 to 1 with values more 1 or less than zero clamped.
	//
	_NODISCARD inline static My_Vector2_t PointToNormalized(const Rect& _rect, const My_Vector2_t& _point) {
		return Vector2(
			(_point.x - _rect.xMin()) / _rect.width(),
			(_point.y - _rect.yMin()) / _rect.height()
		).Clamp01();
	}

public: // Static Properties
	// 
	// Shorthand for writing Rect(0, 0, 0, 0).
	// 
	_NODISCARD inline static Rect zero() {
		return Rect(0, 0, 0, 0);
	}
};

//
// A 2D Rectangle defined by x, y, width, height with integers.
//
class RectInt : public impl::BaseRect<int, Vector2Int> {
public:	// Constructors & Destructors
	//
	// Creates a new Rect.
	// 
	// @param _x:
	//		The minimum X value of the RectInt.
	// @param _y:
	//		The minimum Y value of the RectInt.
	// @param _w:
	//		Width of the rectangle.
	// @param _h:
	//		Height of the rectangle.
	//
	inline explicit RectInt(coord_t _x, coord_t _y, coord_t _w, coord_t _h) 
		: BaseRect(_x, _y, _w, _h) {}

	//
	// Creates a new Rect.
	// 
	// @param _pos:
	//		The position (x, y) of the RectInt.
	// @param _size:
	//		The width (x) and height (y) of the RectInt.
	//
	inline explicit RectInt(const My_Vector2_t& _position, const My_Vector2_t& _size)
		: RectInt(_position.x, _position.y, _size.x, _size.y) {}

	//
	// Creates a new RectInt with position and size of (0, 0).
	//
	inline RectInt() : RectInt(0, 0, 0, 0) {}

public: // Operators Overloads
#if LMK_HAVE_SDL
	//
	// Convert a lmk::RectInt to SDL_Rect.
	//
	_NODISCARD inline operator SDL_Rect() {
		return SDL_Rect{ m_xMin, m_yMin, m_width, m_height };
	}
#endif

public: // Functions
	//
	// Clamps the position and size of the RectInt to the given _bounds.
	// 
	// @param _bounds:
	//		Bounds to clamp the RectInt.
	//
	inline void ClampToBounds(const RectInt& _bounds) {
		setMin(minPos().Clamp(_bounds.minPos(), _bounds.maxPos()));
		setMax(maxPos().Clamp(_bounds.minPos(), _bounds.maxPos()));
	}

public: // Static Properties
	// 
	// Shorthand for writing RectInt(0, 0, 0, 0).
	// 
	_NODISCARD inline static RectInt zero() {
		return RectInt(0, 0, 0, 0);
	}
};
#pragma warning(default : 4244)

// +--------------------------------------------------------------------------------+
// |																				|
// | RAY																			|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Representation of an infinite line starting at origin and going in some direction.
//
struct Ray {
public: // Constructors & Destructors
	//
	// Creates a new Ray.
	// 
	// @param _origin:
	//		The origin point of the ray.
	// @param _direction:
	//		The direction of the ray.
	// 
	inline Ray(const Vector2& _origin, const Vector2& _direction)
		: origin(_origin), direction(_direction) {}

	//
	// Creates a new Ray with origin of (0, 0) and direction of (0, 1).
	//
	inline Ray()
		: Ray(Vector2::zero(), Vector2::up()) {}

public: // Functions
	//
	// Returns a point at distance units along the ray.
	//
	inline Vector2 GetPoint(float _distance) {
		return origin + direction * _distance;
	}

public: // Properties
	Vector2 origin;		// The origin point of the ray.
	Vector2 direction;	// The direction of the ray.
};

// +--------------------------------------------------------------------------------+
// |																				|
// | BOUNDS																			|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Represents an axis aligned bounding box that fully encloses some object.
//
// An axis-aligned bounding box, or AABB for short, is a box aligned with coordinate axes.
// Because the box is never rotated with respect to the axes, it can be defined by 
// just its center and extents, or alternatively by min and max points.
//
struct Bounds {
public: // Constuctors & Destructors
	//
	// Creates a new Bound.
	// 
	// @param _center:
	//		The location of the origin of the Bounds.
	// @param _size:
	//		The dimensions of the Bounds.
	//
	inline explicit Bounds(const Vector2& _center, const Vector2& _size)
		: m_center(_center), m_size(_size), m_extents(_size / 2), m_min(_center - m_extents), m_max(_center + m_extents) {}

	//
	// Creates a new Bounds with center and size of (0, 0).
	//
	inline Bounds() 
		: Bounds(Vector2::zero(), Vector2::zero()) {}

public: // Functions
	//
	// Expand the bounds by increasing its size by _amount along each side.
	// 
	// This function will evenly changes its min and max point to preserve the center of the bounding box.
	//
	inline void Expand(float _amount) {
		setSize(Vector2(m_size.x + _amount, m_size.y + _amount));
	}

	//
	// Expand the bounds by increasing its size by _amount along the corresponding axes.
	// 
	// This function will evenly changes its min and max point to preserve the center of the bounding box.
	//
	inline void Expand(const Vector2& _amount) {
		setSize(m_size + _amount);
	}

	//
	// Grows the Bounds to include the _point.
	// 
	// This function will find the minimum extents 
	//
	inline void Encapsulate(const Vector2& _point) {
		setMinMax(
			Vector2::Min(_point, m_min),
			Vector2::Max(_point, m_max)
		);
	}

	//
	// Grow the bounds to encapsulate the _bounds.
	// 
	// This will preserve the center point of the bounding box and extents its edges
	// to include the _bounds on the bounding box.
	//
	inline void Encapsulate(const Bounds& _bounds) {
		setMinMax(
			Vector2::Min(_bounds.m_min, m_min),
			Vector2::Max(_bounds.m_max, m_max)
		);
	}

	// 
	// The closest point on the bounding box.
	// If _point is inside the bounding box, unmodified _point position will be returned.
	// 
	// @param _point:
	//		Arbitrary point.
	// 
	// @return:
	//		A Vector2 represents the point on the bounding box or inside the bounding box.
	//
	_NODISCARD inline Vector2 ClosestPoint(const Vector2& _point) const noexcept {
		return Vector2(
			LMK_Clamp(_point.x, m_min.x, m_max.x),
			LMK_Clamp(_point.y, m_min.y, m_max.y)
		);
	}

	//
	// Is _point contained in the bounding box?
	//
	_NODISCARD inline bool Contains(const Vector2& _point) const noexcept {
		return (_point.x > m_min.x) && (_point.x < m_max.x) 
			&& (_point.y > m_min.y) && (_point.y < m_max.y);
	}

	//
	// Does another bounding box intersect with this bounding box?
	//
	_NODISCARD inline bool Intersects(const Bounds& _bounds) const noexcept {
		return (m_min.x < _bounds.m_max.x) 
			&& (m_max.x > _bounds.m_min.x)
			&& (m_min.y < _bounds.m_max.y)
			&& (m_max.y > _bounds.m_min.y);
	}

	//
	// Does _ray intersect this bounding box?
	//
	_NODISCARD inline bool IntersectRay(Ray& _ray) noexcept {
		// For the original formula of box - ray intersection, see:
		// https://tavianator.com/2022/ray_box_boundary.html

		float tmin = 0.0f; 
		float tmax = INFINITY;

		// Loop through all dimentions of the bounding box and calculate the min and max
		// Overlapping point between the ray and the bounding box.
		for (int d = 0; d < 2; d++) {
			float t1 = (m_min[d] - _ray.origin[d]) * _ray.direction[d];
			float t2 = (m_max[d] - _ray.origin[d]) * _ray.direction[d];

			tmin = LMK_Max(tmin, LMK_Min(t1, t2));
			tmax = LMK_Min(tmax, LMK_Max(t1, t2));
		}

		return tmin < tmax;
	}

public: // Accessors
	// Get the center of the bounding box.
	_NODISCARD inline Vector2 center() const noexcept {
		return m_center;
	}

	// Get the total size of the box. This is always twice as large as the extents.
	_NODISCARD inline Vector2 size() const noexcept {
		return m_size;
	}

	// Gett the extents of the Bounding Box. This is always half of the size of the Bounds.
	_NODISCARD inline Vector2 extents() const noexcept {
		return m_extents;
	}

	// Get the minimal point of the box. This is always equal to center - extents.
	_NODISCARD inline Vector2 minPos() const noexcept {
		return m_min;
	}

	// Get the maximal point of the box. This is always equal to center + extents.
	_NODISCARD inline Vector2 maxPos() const noexcept {
		return m_max;
	}

public: // Mutators
	// Set the center of the bounding box.
	inline void setCenter(const Vector2& _center) {
		m_center = _center;
		m_min = m_center - m_extents;
		m_max = m_center + m_extents;
	}

	// Set the center of the bounding box. This is always twice as large as the extents.
	inline void setSize(const Vector2& _size) {
		m_size = _size;
		m_extents = m_size / 2;
		m_min = m_center - m_extents;
		m_max = m_center + m_extents;
	}

	// Set the extents of the Bounding Box. This is always half of the size of the Bounds.
	inline void setExtents(const Vector2& _extents) {
		m_extents = _extents;
		m_size = m_extents * 2;
		m_min = m_center - m_extents;
		m_max = m_center + m_extents;
	}

	// Set the minimal point of the box. This is always equal to center - extents.
	inline void setMin(const Vector2& _min) {
		m_min = _min;
		m_size = m_max - m_min;
		m_extents = m_size / 2;
		m_center = m_min + m_extents;
	}

	// Set the maximal point of the box. This is always equal to center + extents.
	inline void setMax(const Vector2& _max) {
		m_max = _max;
		m_size = m_max - m_min;
		m_extents = m_size / 2;
		m_center = m_max - m_extents;
	}

	// Sets the bounds to the min and max value of the box.
	// Using this function is faster than assigning min and max separately.
	inline void setMinMax(const Vector2& _min, const Vector2& _max) {
		m_min = _min;
		m_max = _max;
		m_size = m_max - m_min;
		m_extents = m_size / 2;
		m_center = m_min + m_extents;
	}

private: // Properties
	Vector2 m_center;	// The center of the bounding box.
	Vector2 m_size;		// The total size of the box. This is always twice as large as the extents.
	Vector2 m_extents;	// The extents of the Bounding Box. This is always half of the size of the Bounds.
	Vector2 m_min;		// The minimal point of the box. This is always equal to center - extents.
	Vector2 m_max;		// The maximal point of the box. This is always equal to center + extents.
};

// +--------------------------------------------------------------------------------+
// |																				|
// | COLOR																			|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Representation of RGBA colors.
// 
// Each color component is an integer value with a range from 0 to 255 (uint8_t):
// 
// - Components (r,g,b) define a color in RGB color space. 
// - Alpha component (a) defines transparency with: 0 = completely transparent; 255 = completely opaque.
//
class Color {
public: // Constructors & Destructors
	//
	// 
	//
	inline Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

public: // Operators Overloads
#if LMK_HAVE_SDL
	//
	// Converts a lmk::Color to SDL_Color
	//
	_NODISCARD inline operator SDL_Color() noexcept {
		return SDL_Color{ r, g, b, a };
	}
#endif

	//
	// Access the r, g, b, a components using [0], [1], [2], [3] respectively.
	//
	_NODISCARD inline uint8_t& operator[](uint8_t _index) {
		switch (_index)
		{
		case 0:
			return r;
		case 1:
			return g;
		case 2:
			return b;
		case 3:
			return a;
		default:
			throw std::out_of_range("lmk::Color: index out of range for operator[] (index should be 0, 1, 2 or 3).");
			break;
		}
	}

	inline Color& operator+=(const Color& _right) {
		r += _right.r;
		g += _right.g;
		b += _right.b;
		a += _right.a;
		return *this;
	}

	inline Color& operator-=(const Color& _right) {
		r -= _right.r;
		g -= _right.g;
		b -= _right.b;
		a -= _right.a;
		return *this;
	}

#pragma warning (disable : 4244)
	inline Color& operator*=(float _right) {
		for (uint8_t i = 0; i < 4; i++) {
			(*this)[i] *= _right;
		}
		return *this;
	}

	inline Color& operator/=(float _right) {
		for (uint8_t i = 0; i < 4; i++) {
			(*this)[i] /= _right;
		}
		return *this;
	}

	_NODISCARD inline Color operator+(const Color& _right) const {
		Color temp = *this;
		temp += _right;
		return temp;
	}

	_NODISCARD inline Color operator-(const Color& _right) const {
		Color temp = *this;
		temp -= _right;
		return temp;
	}

	_NODISCARD inline Color operator*(float _right) const {
		Color temp = *this;
		temp *= _right;
		return temp;
	}

	_NODISCARD inline Color operator/(float _right) const {
		Color temp = *this;
		temp /= _right;
		return temp;
	}

public: // Static Functions
	//
	// Creates an RGB colour from HSV input.
	// 
	// @param _h:
	//		Hue [0...1).
	// @param _s:
	//		Saturation [0...1].
	// @param _v:
	//		Brightness value [0...1].
	// 
	// @returns:
	//		Color An opaque colour with HSV matching the input.
	//
	_NODISCARD inline static Color HSVToRGB(float _h, float _s, float _v) {
		// For a reference to the original formula of this conversion, see: 
		// https://web.archive.org/web/20090102212804/http://en.wikipedia.org/wiki/HSL_color_space

		uint8_t hi = LMK_RoundToInt(_h * 6) % 6;
		float f = _h * 6 - LMK_RoundToInt(_h * 6);
		float p = _v * (1 - f);
		float q = _v * (1 - f * _s);
		float t = _v * (1 - (1 - f) * _s);

		switch (hi)
		{
		case 0:
			return Color(std::round(_v * 255), std::round(t * 255), std::round(p * 255), 1);
		case 1:
			return Color(std::round(q * 255), std::round(_v * 255), std::round(p * 255), 1);
		case 2:
			return Color(std::round(p * 255), std::round(_v * 255), std::round(t * 255), 1);
		case 3:
			return Color(std::round(p * 255), std::round(q * 255), std::round(_v * 255), 1);
		case 4:
			return Color(std::round(t * 255), std::round(p * 255), std::round(_v * 255), 1);
		case 5:
			return Color(std::round(_v * 255), std::round(p * 255), std::round(q * 255), 1);
		}
	}

	//
	// Calculates the hue, saturation and value of an RGB input color.
	// 
	// @param _rgb:
	//		An input RGB color.
	// @param _h:
	//		Output variable for hue.
	// @param _s:
	//		Output variable for saturation.
	// @param _v:
	//		Output variable for brightness value.
	// 
	// @returns:
	//		The H, S, and V are output in the range 0 to 1.
	//
	_NODISCARD inline static void RGBToHSV(const Color& _rgb, float& _h, float& _s, float& _v) {
		// For a reference to the original formula of this conversion, see: 
		// https://web.archive.org/web/20090102212804/http://en.wikipedia.org/wiki/HSL_color_space
		
		// normalize rgb value to [0...1].
		float fr	= _rgb.r / UINT8_MAX;
		float fg	= _rgb.g / UINT8_MAX;
		float fb	= _rgb.b / UINT8_MAX;
		// get the min and max value between r, g, and b.
		float min = (fr < fg && fr < fb) ? fr : (fg < fb) ? fg : fb;
		float max	= (fr > fg && fr > fb) ? fr : (fg > fb) ? fg : fb;

		// reset hsv value.
		_h = _s = _v = 0.0f;
		_v = (max + min) / 2.0f;
								
		if (max == min) {	// all values are the same -> 
			_h = _s = 0.0f;	// achromatic color (gray).
		}
		else {
			float d = max - min;
			_s = (_v > 0.5f) ? d / (2.0f - max - min) : d / (max + min);
			if (fr > fg && fr > fb)
				_h = (fg - fb) / d + (fg < fb ? 6.0f : 0.0f);
			else if (fg > fb)
				_h = (fb - fr) / d + 2.0f;
			else
				_h = (fr - fg) / d + 4.0f;
			_h /= 6.0f;
		}
	}

	//
	// Linearly interpolates between colors _a and _b by _t.
	// 
	// _t is clamped between 0 and 1. 
	// When t is 0 returns a. 
	// When t is 1 returns b.
	//
	_NODISCARD inline static Color Lerp(const Color& _a, const Color& _b, float _t) {
		return LMK_Lerp(_a, _b, LMK_Clamp(_t, 0.0f, 1.0f));
	}

	//
	// Linearly interpolates between colors _a and _b by _t.
	// 
	// When t is 0 returns a. 
	// When t is 1 returns b.
	//
	_NODISCARD inline static Color LerpUnclamped(const Color& _a, const Color& _b, float _t) {
		return LMK_Lerp(_a, _b, _t);
	}
#pragma warning (default : 4244)

public: // Static Properties
	//
	// Completely transparent. RGBA is (0, 0, 0, 0).
	//
	_NODISCARD inline static Color clear() {
		return Color(0, 0, 0, 0);
	}

	//
	// Solid black. RGBA is (0, 0, 0, 255).
	//
	_NODISCARD inline static Color black() {
		return Color(0, 0, 0, 255);
	}

	//
	// Solid white. RGBA is (255, 255, 255, 255).
	//
	_NODISCARD inline static Color white() {
		return Color(255, 255, 255, 255);
	}

	//
	// Grey. RGBA is (128, 128, 128, 255).
	//
	_NODISCARD inline static Color grey() {
		return Color(128, 128, 128, 255);
	}

	//
	// Solid red. RGBA is (255, 0, 0, 255).
	//
	_NODISCARD inline static Color red() {
		return Color(255, 0, 0, 255);
	}

	//
	// Solid green. RGBA is (0, 255, 0, 255).
	//
	_NODISCARD inline static Color green() {
		return Color(0, 255, 0, 255);
	}

	//
	// Solid blue. RGBA is (0, 0, 255, 255).
	//
	_NODISCARD inline static Color blue() {
		return Color(0, 0, 255, 255);
	}

	//
	// Cyan. RGBA is (0, 255, 255, 255).
	//
	_NODISCARD inline static Color cyan() {
		return Color(0, 255, 255, 255);
	}

	//
	// Magenta. RGBA is (255, 0, 255, 255).
	//
	_NODISCARD inline static Color magenta() {
		return Color(255, 0, 255, 255);
	}

	//
	// Yellow. RGBA is (255, 235, 4, 255).
	//
	_NODISCARD inline static Color yellow() {
		return Color(255, 255, 0, 255);
	}

public: // Properties
	uint8_t r, g, b, a;
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | OBJECT				 															|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// Base class for all objects LMK Engine can reference.
//
class Object {
public: // Functions
	//
	// Gets the instance ID of the object.
	//
	_NODISCARD inline int GetInstanceID() {
		return m_id;
	}

public: // Static Functions
	//
	// Removes a GameObject, component or asset.
	// 
	// The object _obj is destroyed immediately after the current Update loop, or _t seconds from now if a time is specified. 
	// 
	// If _obj is a Component, this method removes the component from the GameObject and destroys it. 
	// If obj is a GameObject, it destroys the GameObject, all its components and all transform children of the GameObject. 
	// Actual object destruction is always delayed until after the current Update loop, but is always done before rendering.
	// 
	// Note: When destroying MonoBehaviour scripts, LMK calls OnDisable and OnDestroy before the script is removed.
	// 
	// @param _obj:
	//		The object to destroy.
	// @param _t:
	//		The optional amount of time to delay before destroying the object.
	//
	inline static void Destroy(Object* _obj, float _t = 0.0F) {

	}

	//
	//
	//
	inline static void DontDestroyOnLoad(Object _target) {

	}


public: // Properties
	std::string name;	// The name of the object.

private:
	int m_id;
};
LMK_END

#endif // !LMK_COREMDL_2D_H_