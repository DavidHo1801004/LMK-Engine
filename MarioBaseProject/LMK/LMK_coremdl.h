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
	using layer_size_type	= TagsLayersSystem::layer_size_type;

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
	inline LayerMask() {}

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
	// @return
	//		The layer mask created from the _layerNames.
	//
	inline static int GetMask(std::string _layerNames...) {
		return 0;
	}

	//
	// Given a layer number, returns the name of the layer as defined in the TagsLayersSystem.
	// 
	// @return
	//		
	//
	inline static std::string LayerToName(layer_size_type _layer) {
		return "";
	}

	//
	// Given a layer number, returns the name of the layer as defined in the TagsLayersSystem.
	// 
	// @return
	//		
	//
	inline static layer_size_type NameToLayer(std::string _layer) {
		return 0;
	}

public: // Accessors
	// Converts a layer mask value to an integer value.
	_NODISCARD inline layer_size_type value() {
		return m_value;
	}

private:
	layer_size_type m_value;
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
// @tparam Type:
//		Single coordinate as an arithmetic type (e.g. int, float, ...).
//
template <typename Type>
class BaseVector2 {
public:	// Typedef
	// An arithmetic type (i.e, int, float, double,...)
	using base_type = typename Type;
	static_assert(std::is_arithmetic_v<base_type>, "lmk::BaseVector2: Invalid coordinate type.");

public:	// Constructors & Destructors
	//
	// Creates a new BaseVector2 with the given x and y components.
	//
	inline explicit BaseVector2(base_type _x, base_type _y)
		: x(_x), y(_y) {}

public:	// Operators Overloads
	//
	// Returns a formatted string for this vector.
	//
	// @return
	//		A formatted string: {x; y}
	//
	_NODISCARD inline operator std::string() const {
		return "{" + std::to_string(x) + "; " + std::to_string(y) + "}";
	}

	//
	// Access the x or y component using [0] or [1] respectively.
	//
	_NODISCARD constexpr base_type& operator[](int _index) {
		switch (_index) {
		case 0: return x;
		case 1: return y;
		}
		LMK_CORE_ASSERT(false, "lmk::BaseVector2: index out of range for operator[] (index should be 0 or 1).");
	}

	//
	// Access the x or y component using [0] or [1] respectively.
	//
	_NODISCARD constexpr base_type operator[](int _index) const {
		switch (_index) {
		case 0: return x;
		case 1: return y;
		}
		LMK_CORE_ASSERT(false, "lmk::BaseVector2: index out of range for operator[] (index should be 0 or 1).");
	}

	template <typename Right_Type>
	_NODISCARD constexpr bool operator==(const BaseVector2<Right_Type>& _right) const noexcept {
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
		return std::sqrt(x * x + y * y);
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
		return x * x + y * y;
	}

public: // Properties
	base_type x;	// The X component of the vector.
	base_type y;	// The Y component of the vector.
};
IMPL_END

// Forward declaration.
class Vector3;

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
	inline explicit Vector2(base_type _x, base_type _y)
		: BaseVector2(_x, _y) {}

	//
	// Creates a new Vector2 with coordinate of (0, 0).
	//
	inline Vector2()
		: Vector2(0, 0) {}

public: // Operators Overloads
#if LMK_HAVE_SDL
	//
	// Converts a lmk::Vector2 to SDL_FPoint.
	//
	_NODISCARD inline operator SDL_FPoint() {
		return SDL_FPoint{x, y};
	}
#endif

	//
	// Converts a Vector2 to a Vector3.
	// 
	// A Vector2 can be implicitly converted into a Vector3. (The z is set to zero in the result).
	//
	_NODISCARD inline operator Vector3();

	inline Vector2& operator+=(Vector2 _right) noexcept {
		x += _right.x;
		y += _right.y;
		return *this;
	}

	inline Vector2& operator-=(Vector2 _right) noexcept {
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

	_NODISCARD inline Vector2 operator+(Vector2 _right) const {
		Vector2 temp = *this;
		temp += _right;
		return temp;
	}

	_NODISCARD inline Vector2 operator-(Vector2 _right) const {
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
	// NOTE: this function does not modify the current vector. 
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
	// NOTE: This function will change the current vector. If you want to keep the current vector unchanged, use normalized().
	//		
	inline Vector2& Normalize() {
		*this /= magnitude();
		return *this;
	}

	// 
	// Multiplies every component of this vector by the same component of _scale.
	// 
	inline Vector2& Scale(Vector2 _scale) noexcept {
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
	_NODISCARD inline static Vector2 Direction(Vector2 _from, Vector2 _to) noexcept {
		return (_to - _from).normalized();
	}

	// 
	// Distance(a, b) is the same as (a - b).magnitude().
	// 
	// @return
	//		The distance between _a and _b.
	// 
	_NODISCARD inline static float Distance(Vector2 _a, Vector2 _b) noexcept {
		return (_b - _a).magnitude();
	}

	// 
	// Clamp the magnitude of a Vector2.
	// 
	// @return
	//		A copy of _vector with its magnitude clamped to _maxLength.
	// 
	_NODISCARD inline static Vector2 ClampMagnitude(Vector2 _vector, float _maxLength) {
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
	_NODISCARD inline static float Dot(Vector2 _lhs, Vector2 _rhs) noexcept {
		return _lhs.x * _rhs.x + _lhs.y * _rhs.y;
	}

	// 
	// Gets the unsigned angle in degrees between _from and _to.
	//
	// The angle returned is the unsigned angle between the two vectors.
	//	
	// NOTE: The angle returned will always be between 0 and 180 degrees, 
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
	_NODISCARD inline static float Angle(Vector2 _from, Vector2 _to) {
		return std::abs(SignedAngle(_from, _to));
	}

	// 
	// Gets the signed angle in degrees between _from and _to.
	//	
	// The angle returned is the signed angle between the two vectors.
	//	
	// NOTE: The angle returned will always be between -180 and 180 degrees,
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
	_NODISCARD inline static float SignedAngle(Vector2 _from, Vector2 _to) {
		return LMK_RtoD(std::atan2(_from.x * _to.y - _from.y * _to.x, _from.x * _to.x + _from.y * _to.y));
	}

	// 
	// Returns a vector that is made from the largest components of two vectors.
	// 
	_NODISCARD inline static Vector2 Max(Vector2 _a, Vector2 _b) noexcept {
		return Vector2(
			LMK_Max(_a.x, _b.x),
			LMK_Max(_a.y, _b.y)
		);
	}

	// 
	// Returns a vector that is made from the smallest components of two vectors.
	// 
	_NODISCARD inline static Vector2 Min(Vector2 _a, Vector2 _b) noexcept {
		return Vector2(
			LMK_Min(_a.x, _b.x),
			LMK_Min(_a.y, _b.y)
		);
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
	_NODISCARD inline static Vector2 Lerp(Vector2 _from, Vector2 _to, float _t) noexcept {
		return LMK_Lerp(_from, _to, LMK_Clamp(_t, 0.0f, 1.0f));
	}

	// 
	// Linearly interpolates between vectors _a and _b by _t.
	// 
	// When _t = 0 returns _a.
	// When _t = 1 return _b.
	// When _t = 0.5 returns the midpoint of _a and _b.
	// 
	_NODISCARD inline static Vector2 LerpUnclamped(Vector2 _from, Vector2 _to, float _t) noexcept {
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
	_NODISCARD inline static Vector2 MoveTowards(Vector2 _current, Vector2 _target, float _maxDistanceDelta) {
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
	_NODISCARD inline static Vector2 Perpendicular(Vector2 _inDirection) {
		base_type x = std::cos(LMK_DtoR(90)) * _inDirection.x - std::sin(LMK_DtoR(90)) * _inDirection.y;
		base_type y = std::sin(LMK_DtoR(90)) * _inDirection.x + std::cos(LMK_DtoR(90)) * _inDirection.y;
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
	_NODISCARD inline static Vector2 Reflect(Vector2 _inDirection, Vector2 _inNormal) {
		return _inDirection - (_inNormal * 2) * Dot(_inDirection, _inNormal);
	}

	// 
	// Multiplies two vectors component-wise.
	// 
	// Every component in the result is a component of _a multiplied by the same component of _b.
	// 
	_NODISCARD inline static Vector2 Scale(Vector2 _a, Vector2 _b) {
		base_type x = _a.x * _b.x;
		base_type y = _a.y * _b.y;
		return Vector2(x, y);
	}

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

// Vector2:
// Non-member Operators Overloads

_NODISCARD inline Vector2 operator*(float _left, Vector2 _right) {
	Vector2 temp = _right;
	temp *= _left;
	return temp;
}

// 
// Representation of 2D vectors and points using integers.
// 
class Vector2Int : public impl::BaseVector2<int> {
private:
	using underlying_type = BaseVector2<int>;

public: // Constructors & Destructors
	//
	// Creates a new Vector2Int.
	// 
	// @param _x:
	//		The X coordinate of the vector.
	// @param _y:
	//		The Y coordinate of the vector.
	//
	inline explicit Vector2Int(base_type _x, base_type _y)
		: BaseVector2(_x, _y) {}

	//
	// Creates a new Vector2Int with coordinate of (0, 0).
	//
	inline Vector2Int()
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

	inline Vector2Int& operator+=(Vector2Int _right) noexcept {
		x += _right.x;
		y += _right.y;
		return *this;
	}

	inline Vector2Int& operator-=(Vector2Int _right) noexcept {
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

	_NODISCARD inline Vector2Int operator+(Vector2Int _right) const noexcept {
		Vector2Int temp = *this;
		temp += _right;
		return temp;
	}

	_NODISCARD inline Vector2Int operator-(Vector2Int _right) const noexcept {
		Vector2Int temp = *this;
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
	inline Vector2Int& Clamp(Vector2Int _min, Vector2Int _max) noexcept {
		x = LMK_Clamp(x, _min.x, _max.x);
		y = LMK_Clamp(y, _min.y, _max.y);
		return *this;
	}

	// 
	// Multiplies every component of this vector by the same component of _scale.
	// 
	_NODISCARD inline void Scale(Vector2Int _scale) {
		x *= _scale.x;
		y *= _scale.y;
	}

public: // Static Functions
	// 
	// Converts a Vector2 to a Vector2Int by doing a std::ceil() to each value.
	//
	// As there is a conversion of float to integer, there is a loss of precision.
	// 
	_NODISCARD inline static Vector2Int CeilToInt(Vector2 _v) noexcept {
		return Vector2Int(std::ceil(_v.x), std::ceil(_v.y));
	}

	// 
	// Converts a Vector2 to a Vector2Int by doing a std::floor() to each value.
	// 
	// As there is a conversion of float to integer, there is a loss of precision.
	// 
	_NODISCARD inline static Vector2Int FloorToInt(Vector2 _v) noexcept {
		return Vector2Int(std::floor(_v.x), std::floor(_v.y));
	}

	// 
	// Converts a Vector2 to a Vector2Int by doing a std::round() to each value.
	// 
	// As there is a conversion of float to integer, there is a loss of precision.
	// 
	_NODISCARD inline static Vector2Int RoundToInt(Vector2 _v) noexcept {
		return Vector2Int(std::round(_v.x), std::round(_v.y));
	}

	// 
	// Distance(a, b) is the same as (a - b).magnitude().
	// 
	// @return
	//		The distance between _a and _b.
	// 
	_NODISCARD inline static float Distance(Vector2Int _a, Vector2Int _b) noexcept {
		return (_b - _a).magnitude();
	}

	// 
	// Returns a vector that is made from the largest components of two vectors.
	// 
	// @return
	//		A copy of the larger Vector2Int.
	// 
	_NODISCARD inline static Vector2Int Max(Vector2Int _a, Vector2Int _b) noexcept {
		return (_a.sqrMagnitude() > _b.sqrMagnitude()) ? _a : _b;
	}

	// 
	// Returns a vector that is made from the smallest components of two vectors.
	// 
	// @return
	//		A copy of the smaller Vector2Int.
	// 
	_NODISCARD inline static Vector2Int Min(Vector2Int _a, Vector2Int _b) noexcept {
		return (_a.sqrMagnitude() < _b.sqrMagnitude()) ? _a : _b;
	}

	// 
	// Multiplies two vectors component-wise.
	// 
	// Every component in the result is a component of _a multiplied by the same component of _b.
	// 
	_NODISCARD inline static Vector2Int Scale(Vector2Int _a, Vector2Int _b) {
		base_type x = _a.x * _b.x;
		base_type y = _a.y * _b.y;
		return Vector2Int(x, y);
	}

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

	// 
	// Shorthand for writing Vector2Int(0, 0).
	// 
	_NODISCARD inline static Vector2Int zero() noexcept {
		return Vector2Int(0, 0);
	}
};

// Vector2Int
// Non-member Operators Overloads

_NODISCARD inline Vector2Int operator*(float _left, Vector2Int _right) {
	Vector2Int temp = _right;
	temp *= _left;
	return temp;
}


// +--------------------------------------------------------------------------------+
// |																				|
// | 3D VECTOR																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Base class for creating 3D vector classes.
// 
// This structure is used throughout LMK Engine to generate 2D transformation matrix.
//
template <typename Type>
class BaseVector3 {
public: // Typedef
	// An arithmetic type (i.e, int, float, double,...)
	using base_type = typename Type;
	static_assert(std::is_arithmetic_v<base_type>, "lmk::BaseVector3: Invalid coordinate type.");

public: // Constructors & Destructors
	//
	// Creates a new BaseVector3 with the given x, y, and z components.
	//
	inline BaseVector3(base_type _x, base_type _y, base_type _z)
		: x(_x), y(_y), z(_z) {}

	//
	// Creates a new BaseVector3 with the given x and y components.
	//
	inline BaseVector3(base_type _x, base_type _y)
		: x(_x), y(_y), z(0) {}

public:	// Operators Overloads
	//
	// Returns a formatted string for this vector.
	//
	// @return
	//		A formatted string: {x; y; z}
	//
	_NODISCARD inline operator std::string() const {
		return "{" + std::to_string(x) + "; " + std::to_string(y) + "; " + std::to_string(z) + "}";
	}

	//
	// Access the x, y, or z component using [0], [1], or [2] respectively.
	//
	_NODISCARD constexpr base_type& operator[](int _index) {
		switch (_index) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		}
		LMK_CORE_ASSERT(false, "lmk::BaseVector3: index out of range for operator[] (index should be 0, 1, or 2).");
	}

	//
	// Access the x, y, or z component using [0], [1], or [2] respectively.
	//
	_NODISCARD constexpr base_type operator[](int _index) const {
		switch (_index) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		}
		LMK_CORE_ASSERT(false, "lmk::BaseVector3: index out of range for operator[] (index should be 0, 1, or 2).");
	}

	template <typename Right_Type>
	_NODISCARD constexpr bool operator==(const BaseVector3<Right_Type>& _right) const noexcept {
		return (x == _right.x) && (y == _right.y) && (z == _right.z);
	}

public: // Functions
	//
	// The length of this vector.
	//
	// @return
	//		Square root of (x * x + y * y + z * z).
	//
	_NODISCARD constexpr float magnitude() const noexcept {
		return std::sqrt(x * x + y * y + z * z);
	}

	//
	// The squared length of this vector.
	//	
	// Calculating the squared magnitude instead of the magnitude is much faster.
	// Often if you are comparing magnitudes of two vectors you can just compare their squared magnitudes.
	//		
	// @return
	//		The result of (x * x + y * y + z * z).
	//
	_NODISCARD constexpr float sqrMagnitude() const noexcept {
		return x * x + y * y + z * z;
	}

public: // Properties
	base_type x;	// The X component of the vector.
	base_type y;	// The Y component of the vector.
	base_type z;	// The Z component of the vector.
};

//
// Representation of 3D vectors and points.
//
class Vector3 : public BaseVector3<float> {
public:	// Constructors & Destructors
	//
	// Creates a new Vector2 with the given x, y, and z components.
	//
	inline explicit Vector3(base_type _x, base_type _y, base_type _z)
		: BaseVector3(_x, _y, _z) {}

	//
	// Creates a new Vector2 with the given x and y components.
	//
	inline explicit Vector3(base_type _x, base_type _y)
		: BaseVector3(_x, _y) {}

	//
	// Creates a new Vector3 with coordinate of (0, 0, 0).
	//
	inline Vector3()
		: Vector3(0, 0, 0) {}

public: // Operators Overloads
	//
	// Converts a Vector3 to a Vector2.
	// 
	// A Vector3 can be implicitly converted into a Vector2. (The z is discarded).
	//
	_NODISCARD inline explicit operator Vector2() {
		return Vector2(x, y);
	}

	inline Vector3& operator+=(const Vector3& _right) noexcept {
		x += _right.x;
		y += _right.y;
		z += _right.z;
		return *this;
	}

	inline Vector3& operator-=(const Vector3& _right) noexcept {
		x -= _right.x;
		y -= _right.y;
		z -= _right.z;
		return *this;
	}

	inline Vector3& operator/=(float _right) {
		x /= _right;
		y /= _right;
		z /= _right;
		return *this;
	}

	inline Vector3& operator*=(float _right) noexcept {
		x *= _right;
		y *= _right;
		z *= _right;
		return *this;
	}

	_NODISCARD inline Vector3 operator+(const Vector3& _right) const {
		Vector3 temp = *this;
		temp += _right;
		return temp;
	}

	_NODISCARD inline Vector3 operator-(const Vector3& _right) const {
		Vector3 temp = *this;
		temp -= _right;
		return temp;
	}

	_NODISCARD inline Vector3 operator/(float _right) const {
		Vector3 temp = *this;
		temp /= _right;
		return temp;
	}

	_NODISCARD inline Vector3 operator*(float _right) const {
		Vector3 temp = *this;
		temp *= _right;
		return temp;
	}

public: // Functions
	// 
	// The normalized vector has a magnitude of 1 and is in the same direction as the current vector.
	// 
	// NOTE: this function does not modify the current vector. 
	// To modify and normalize the current vector, use the Normalize() function.
	// 
	// @return
	//		A normalized vector based on the current vector.
	// 
	_NODISCARD inline Vector3 normalized() const {
		return (sqrMagnitude() > 0) ? *this / magnitude() : *this;
	}

	// 
	// Makes this vector have a magnitude of 1.
	//		
	// When normalized, a vector keeps the same direction but its length is 1.0.
	// NOTE: This function will change the current vector. If you want to keep the current vector unchanged, use normalized().
	//		
	inline Vector3& Normalize() {
		*this /= magnitude();
		return *this;
	}

	// 
	// Multiplies every component of this vector by the same component of _scale.
	// 
	inline Vector3& Scale(const Vector3& _scale) noexcept {
		x *= _scale.x;
		y *= _scale.y;
		z *= _scale.z;
		return *this;
	}

	// 
	// Clamp every component of this vector to [0...1].
	// 
	inline Vector3& Clamp01() noexcept {
		x = LMK_Clamp(x, 0, 1);
		y = LMK_Clamp(y, 0, 1);
		z = LMK_Clamp(z, 0, 1);
		return *this;
	}

public: // Static Functions
	// 
	// Get the direction from one Vector2 to another by using: (_to - _from).
	// 
	// @return
	//		A normalized Vector2 represents the calculated direction.
	// 
	_NODISCARD inline static Vector3 Direction(const Vector3& _from, const Vector3& _to) noexcept {
		return (_to - _from).normalized();
	}

	// 
	// Distance(a, b) is the same as (a - b).magnitude().
	// 
	// @return
	//		The distance between _a and _b.
	// 
	_NODISCARD inline static float Distance(const Vector3& _a, const Vector3& _b) noexcept {
		return (_b - _a).magnitude();
	}

	// 
	// Clamp the magnitude of a Vector2.
	// 
	// @return
	//		A copy of _vector with its magnitude clamped to _maxLength.
	// 
	_NODISCARD inline static Vector3 ClampMagnitude(const Vector3& _vector, float _maxLength) {
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
	_NODISCARD inline static float Dot(const Vector3& _lhs, const Vector3& _rhs) noexcept {
		return _lhs.x * _rhs.x + _lhs.y * _rhs.y + _lhs.z * _rhs.z;
	}

	// 
	// Cross Product of two vectors..
	//
	// The cross product of two vectors results in a third vector which is perpendicular to the two input vectors. 
	// The result's magnitude is equal to the magnitudes of the two inputs multiplied together and then multiplied 
	// by the sine of the angle between the inputs. 
	// 
	// (You can determine the direction of the result vector using the "left hand rule")
	// 
	// @return
	//		Returns (_lhs x _rhs).
	// 
	_NODISCARD inline static Vector3 Cross(const Vector3& _lhs, const Vector3& _rhs) noexcept {
		Vector3 result;
		result.x = _lhs.y * _rhs.z - _lhs.z * _rhs.y;
		result.y = _lhs.z * _rhs.x - _lhs.x * _rhs.z;
		result.x = _lhs.x * _rhs.y - _lhs.y * _rhs.x;
		return result;
	}

	// 
	// Gets the unsigned angle in degrees between _from and _to.
	//
	// The angle returned is the unsigned angle between the two vectors.
	//	
	// NOTE: The angle returned will always be between 0 and 180 degrees, 
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
	_NODISCARD inline static float Angle(const Vector3& _from, const Vector3& _to) {
		return std::abs(SignedAngle(_from, _to));
	}

	// 
	// Gets the signed angle in degrees between _from and _to.
	// 
	// The angle returned is the signed angle between the two vectors.
	//	
	// NOTE: The angle returned will always be between -180 and 180 degrees,
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
	_NODISCARD inline static float SignedAngle(const Vector3& _from, const Vector3& _to) {
		return LMK_RtoD(std::atan2(Cross(_from, _to).magnitude(), Dot(_from, _to)));
	}

	// 
	// Returns a vector that is made from the largest components of two vectors.
	// 
	_NODISCARD inline static Vector3 Max(const Vector3& _a, const Vector3& _b) noexcept {
		return Vector3(
			LMK_Max(_a.x, _b.x),
			LMK_Max(_a.y, _b.y),
			LMK_Max(_a.z, _b.z)
		);
	}

	// 
	// Returns a vector that is made from the smallest components of two vectors.
	// 
	_NODISCARD inline static Vector3 Min(const Vector3& _a, const Vector3& _b) noexcept {
		return Vector3(
			LMK_Min(_a.x, _b.x),
			LMK_Min(_a.y, _b.y),
			LMK_Min(_a.z, _b.z)
		);
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
	_NODISCARD inline static Vector3 Lerp(const Vector3& _from, const Vector3& _to, float _t) noexcept {
		return LMK_Lerp(_from, _to, LMK_Clamp(_t, 0.0f, 1.0f));
	}

	// 
	// Linearly interpolates between vectors _a and _b by _t.
	// 
	// When _t = 0 returns _a.
	// When _t = 1 return _b.
	// When _t = 0.5 returns the midpoint of _a and _b.
	// 
	_NODISCARD inline static Vector3 LerpUnclamped(const Vector3& _from, const Vector3& _to, float _t) noexcept {
		return LMK_Lerp(_from, _to, _t);
	}

	// 
	// Moves a point current towards target.
	//		
	// This is essentially the same as Vector3::Lerp but instead the function will ensure 
	// that the distance never exceeds maxDistanceDelta. 
	//		
	// Negative values of maxDistanceDelta pushes the vector away from target.
	// 
	_NODISCARD inline static Vector3 MoveTowards(const Vector3& _current, const Vector3& _target, float _maxDistanceDelta) {
		if (Distance(_current, _target) <= _maxDistanceDelta)
			return _target;
		return _current + (Direction(_current, _target) * _maxDistanceDelta);
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
	_NODISCARD inline static Vector3 Reflect(const Vector3& _inDirection, const Vector3& _inNormal) {
		return _inDirection - (_inNormal * 2) * Dot(_inDirection, _inNormal);
	}

	// 
	// Multiplies two vectors component-wise.
	// 
	// Every component in the result is a component of _a multiplied by the same component of _b.
	// 
	_NODISCARD inline static Vector3 Scale(const Vector3& _a, const Vector3& _b) {
		base_type x = _a.x * _b.x;
		base_type y = _a.y * _b.y;
		base_type z = _a.z * _b.z;
		return Vector3(x, y, z);
	}

	//
	// Projects a vector onto another vector.
	// 
	// @return
	//		_a projected onto _b.
	// 
	_NODISCARD inline static Vector3 Project(const Vector3& _a, const Vector3& _b) {
		return _b * (Dot(_a, _b) / _b.sqrMagnitude());
	}

	//
	// Projects a vector onto a plane.
	// 
	// For a given plane described by _planeNormal and a given vector _vector, 
	// Vector3.ProjectOnPlane generates a new vector orthogonal to _planeNormal and parallel to the plane. 
	// 
	// NOTE: _planeNormal does not need to be normalized.
	// 
	// @param _vector:
	//		The vector to project on the plane.
	// @param _planeNormal:
	//		The normal which defines the plane to project on.
	// 
	// @return
	//		The orthogonal projection of vector on the plane.
	// 
	_NODISCARD inline static Vector3 ProjectOnPlane(const Vector3& _vector, const Vector3& _planeNormal) {
		Vector3 projection;
		float dotProduct = Dot(_vector, _planeNormal);
		for (size_t i = 0; i < 3; i++) {
			projection[i] = _vector[i] - dotProduct * _planeNormal[i];
		}
		return projection;
	}

	// 
	// Shorthand for writing Vector3(0, 1, 0).
	// 
	_NODISCARD inline static Vector3 up() noexcept {
		return Vector3(0, 1, 0);
	}

	// 
	// Shorthand for writing Vector3(0, -1, 0).
	// 
	_NODISCARD inline static Vector3 down() noexcept {
		return Vector3(0, -1, 0);
	}

	// 
	// Shorthand for writing Vector3(1, 0, 0).
	// 
	_NODISCARD inline static Vector3 right() noexcept {
		return Vector3(1, 0, 0);
	}

	// 
	// Shorthand for writing Vector3(-1, 0, 0).
	// 
	_NODISCARD inline static Vector3 left() noexcept {
		return Vector3(-1, 0, 0);
	}

	// 
	// Shorthand for writing Vector3(0, 0, 1).
	// 
	_NODISCARD inline static Vector3 forward() noexcept {
		return Vector3(1, 1, 1);
	}

	// 
	// Shorthand for writing Vector3(0, 0, -1).
	// 
	_NODISCARD inline static Vector3 back() noexcept {
		return Vector3(1, 1, 1);
	}

	// 
	// Shorthand for writing Vector3(1, 1, 1).
	// 
	_NODISCARD inline static Vector3 one() noexcept {
		return Vector3(1, 1, 1);
	}

	// 
	// Shorthand for writing Vector3(0, 0, 0).
	// 
	_NODISCARD inline static Vector3 zero() noexcept {
		return Vector3(0, 0, 0);
	}
};

// Vector3:
// Non-member Operators Overloads

_NODISCARD inline Vector3 operator*(float _left, const Vector3& _right) {
	Vector3 temp = _right;
	temp *= _left;
	return temp;
}

// Vector2:
// Member Operators Overloads implementation.

_NODISCARD inline Vector2::operator Vector3() {
	return Vector3(x, y, 0);
}

//
// Representation of 3D vectors and points in integers.
//
class Vector3Int : public BaseVector3<int> {
public:	// Constructors & Destructors
	//
	// Creates a new Vector2 with the given x, y, and z components.
	//
	inline explicit Vector3Int(base_type _x, base_type _y, base_type _z)
		: BaseVector3(_x, _y, _z) {}

	//
	// Creates a new Vector2 with the given x and y components.
	//
	inline explicit Vector3Int(base_type _x, base_type _y)
		: BaseVector3(_x, _y) {}

	//
	// Creates a new Vector3 with coordinate of (0, 0, 0).
	//
	inline Vector3Int()
		: Vector3Int(0, 0, 0) {}

public: // Operators Overloads
	//
	// Converts a Vector3Int to Vector2Int.
	// 
	// Vector3Ints can be explicitly converted to Vector2Int (z is discarded).
	//
	_NODISCARD inline explicit operator Vector2Int() {
		return Vector2Int(x, y);
	}

	//
	// Converts a Vector3Int to Vector3.
	//
	_NODISCARD inline operator Vector3() {
		return Vector3(x, y, z);
	}

	inline Vector3Int& operator+=(const Vector3Int& _right) noexcept {
		x += _right.x;
		y += _right.y;
		z += _right.z;
		return *this;
	}

	inline Vector3Int& operator-=(const Vector3Int& _right) noexcept {
		x -= _right.x;
		y -= _right.y;
		z -= _right.z;
		return *this;
	}

	inline Vector3Int& operator/=(int _right) {
		x /= _right;
		y /= _right;
		z /= _right;
		return *this;
	}

	inline Vector3Int& operator*=(int _right) noexcept {
		x *= _right;
		y *= _right;
		z *= _right;
		return *this;
	}

	_NODISCARD inline Vector3Int operator+(const Vector3Int& _right) const {
		Vector3Int temp = *this;
		temp += _right;
		return temp;
	}

	_NODISCARD inline Vector3Int operator-(const Vector3Int& _right) const {
		Vector3Int temp = *this;
		temp -= _right;
		return temp;
	}

	_NODISCARD inline Vector3Int operator/(int _right) const {
		Vector3Int temp = *this;
		temp /= _right;
		return temp;
	}

	_NODISCARD inline Vector3Int operator*(int _right) const {
		Vector3Int temp = *this;
		temp *= _right;
		return temp;
	}

public: // Functions
	// 
	// Converts a Vector2 to a Vector2Int by doing a std::ceil() to each value.
	//
	// As there is a conversion of float to integer, there is a loss of precision.
	// 
	_NODISCARD inline static Vector3Int CeilToInt(const Vector3& _v) noexcept {
		return Vector3Int(std::ceil(_v.x), std::ceil(_v.y), std::ceil(_v.z));
	}

	// 
	// Converts a Vector2 to a Vector2Int by doing a std::floor() to each value.
	// 
	// As there is a conversion of float to integer, there is a loss of precision.
	// 
	_NODISCARD inline static Vector3Int FloorToInt(const Vector3& _v) noexcept {
		return Vector3Int(std::floor(_v.x), std::floor(_v.y), std::floor(_v.z));
	}

	// 
	// Converts a Vector2 to a Vector2Int by doing a std::round() to each value.
	// 
	// As there is a conversion of float to integer, there is a loss of precision.
	// 
	_NODISCARD inline static Vector3Int RoundToInt(const Vector3& _v) noexcept {
		return Vector3Int(std::round(_v.x), std::round(_v.y), std::round(_v.z));
	}

	// 
	// Multiplies every component of this vector by the same component of _scale.
	// 
	inline Vector3Int& Scale(const Vector3Int& _scale) noexcept {
		x *= _scale.x;
		y *= _scale.y;
		z *= _scale.z;
		return *this;
	}

	// 
	// Clamps the Vector3Int to the bounds given by min and max.
	// 
	inline Vector3Int& Clamp(const Vector3Int& _min, const Vector3Int& _max) noexcept {
		x = LMK_Clamp(x, _min.x, _max.x);
		y = LMK_Clamp(y, _min.y, _max.y);
		z = LMK_Clamp(z, _min.z, _max.z);
		return *this;
	}

public: // Static Functions


	// 
	// Distance(a, b) is the same as (a - b).magnitude().
	// 
	// @return
	//		The distance between _a and _b.
	// 
	_NODISCARD inline static float Distance(const Vector3& _a, const Vector3& _b) noexcept {
		return (_b - _a).magnitude();
	}

	// 
	// Returns a vector that is made from the largest components of two vectors.
	// 
	_NODISCARD inline static Vector3 Max(const Vector3& _a, const Vector3& _b) noexcept {
		return Vector3(
			LMK_Max(_a.x, _b.x),
			LMK_Max(_a.y, _b.y),
			LMK_Max(_a.z, _b.z)
		);
	}

	// 
	// Returns a vector that is made from the smallest components of two vectors.
	// 
	_NODISCARD inline static Vector3 Min(const Vector3& _a, const Vector3& _b) noexcept {
		return Vector3(
			LMK_Min(_a.x, _b.x),
			LMK_Min(_a.y, _b.y),
			LMK_Min(_a.z, _b.z)
		);
	}

	// 
	// Multiplies two vectors component-wise.
	// 
	// Every component in the result is a component of _a multiplied by the same component of _b.
	// 
	_NODISCARD inline static Vector3 Scale(const Vector3& _a, const Vector3& _b) {
		base_type x = _a.x * _b.x;
		base_type y = _a.y * _b.y;
		base_type z = _a.z * _b.z;
		return Vector3(x, y, z);
	}



	// 
	// Shorthand for writing Vector3(0, 1, 0).
	// 
	_NODISCARD inline static Vector3Int up() noexcept {
		return Vector3Int(0, 1, 0);
	}

	// 
	// Shorthand for writing Vector3(0, -1, 0).
	// 
	_NODISCARD inline static Vector3Int down() noexcept {
		return Vector3Int(0, -1, 0);
	}

	// 
	// Shorthand for writing Vector3(1, 0, 0).
	// 
	_NODISCARD inline static Vector3Int right() noexcept {
		return Vector3Int(1, 0, 0);
	}

	// 
	// Shorthand for writing Vector3(-1, 0, 0).
	// 
	_NODISCARD inline static Vector3Int left() noexcept {
		return Vector3Int(-1, 0, 0);
	}

	// 
	// Shorthand for writing Vector3(0, 0, 1).
	// 
	_NODISCARD inline static Vector3Int forward() noexcept {
		return Vector3Int(1, 1, 1);
	}

	// 
	// Shorthand for writing Vector3(0, 0, -1).
	// 
	_NODISCARD inline static Vector3Int back() noexcept {
		return Vector3Int(1, 1, 1);
	}

	// 
	// Shorthand for writing Vector3(1, 1, 1).
	// 
	_NODISCARD inline static Vector3Int one() noexcept {
		return Vector3Int(1, 1, 1);
	}

	// 
	// Shorthand for writing Vector3(0, 0, 0).
	// 
	_NODISCARD inline static Vector3Int zero() noexcept {
		return Vector3Int(0, 0, 0);
	}
};

// Vector3Int:
// Non-member Operators Overloads
_NODISCARD inline Vector3Int operator*(int _left, const Vector3Int& _right) {
	Vector3Int temp = _right;
	temp *= _left;
	return temp;
}


// +--------------------------------------------------------------------------------+
// |																				|
// | 4D VECTOR																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Representation of four-dimensional vectors.
// 
// This structure is used in some places to represent four component vectors (e.g., mesh tangents, parameters for shaders).
//
class Vector4 {

	//
	// This is a placeholder for future implementations of 3D transformation and texture materials.
	//

public: // Typedef
	using base_type = float;

public: // Constructors && Destructors
	//
	// Creates a new Vector4 with given x, y, z, w components.
	//
	inline Vector4(base_type _x, base_type _y, base_type _z, base_type _w)
		: x(_x), y(_y), z(_z), w(_w) {}

	//
	// Creates a new Vector4 with given x, y, z components and sets w to zero.
	//
	inline Vector4(base_type _x, base_type _y, base_type _z)
		: Vector4(_x, _y, _z, 0) {}

	//
	// Creates a new Vector4 with given x, y components and sets z and w to zero.
	//
	inline Vector4(base_type _x, base_type _y)
		: Vector4(_x, _y, 0) {}

	//
	// Creates a new Vector4 and sets all component to zero.
	//
	inline Vector4() = default;

public: // Operators Overloads
	//
	// Returns a formatted string for this vector.
	//
	// @return
	//		A formatted string: {x; y; z; w}
	//
	_NODISCARD inline operator std::string() const {
		return "{" + std::to_string(x) + "; " + std::to_string(y) + "; " + std::to_string(z) + "; " + std::to_string(w) + "}";
	}

	//
	// Access the x, y, z, w components using [0], [1], [2], [3] respectively.
	//
	_NODISCARD inline base_type& operator[](int _index) {
		switch (_index) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}
		LMK_CORE_ASSERT(false, "lmk::Vector4: index out of range for operator[] (index should be 0, 1, 2 or 3).");
	}

	//
	// Access the x, y, z, w components using [0], [1], [2], [3] respectively.
	//
	_NODISCARD inline base_type operator[](int _index) const {
		switch (_index) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		}
		LMK_CORE_ASSERT(false, "lmk::Vector4: index out of range for operator[] (index should be 0, 1, 2 or 3).");
	}

	_NODISCARD constexpr bool operator==(const Vector4& _right) const noexcept {
		return (x == _right.x) && (y == _right.y) && (z == _right.z) && (w == _right.w);
	}

	inline Vector4& operator+=(Vector4 _right) noexcept {
		x += _right.x;
		y += _right.y;
		return *this;
	}

	inline Vector4& operator-=(Vector4 _right) noexcept {
		x -= _right.x;
		y -= _right.y;
		return *this;
	}

	inline Vector4& operator/=(float _right) {
		x /= _right;
		y /= _right;
		return *this;
	}

	inline Vector4& operator*=(float _right) noexcept {
		x *= _right;
		y *= _right;
		return *this;
	}

	_NODISCARD inline Vector4 operator+(Vector4 _right) const noexcept {
		Vector4 temp = *this;
		temp += _right;
		return temp;
	}

	_NODISCARD inline Vector4 operator-(Vector4 _right) const noexcept {
		Vector4 temp = *this;
		temp -= _right;
		return temp;
	}

	_NODISCARD inline Vector4 operator/(float _right) const {
		Vector4 temp = *this;
		temp /= _right;
		return temp;
	}

	_NODISCARD inline Vector4 operator*(float _right) const noexcept {
		Vector4 temp = *this;
		temp *= _right;
		return temp;
	}

public: // Functions
	//
	// Get the length of this vector.
	// 
	// @return 
	//		Square root of (x * x + y * y + z * z + w * w).
	//
	_NODISCARD inline base_type magnitude() const noexcept {
		return std::sqrt((x * x + y * y + z * z + w * w));
	}

	//
	// Get the squared length of this vector.
	// 
	// Calculating the squared magnitude instead of the magnitude is much faster.
	// Often if you are comparing magnitudes of two vectors you can just compare their squared magnitudes.
	// 
	// @return
	//		The result of (x * x + y * y + z * z + w * w).
	//
	_NODISCARD inline base_type sqrMagnitude() const noexcept {
		return x * x + y * y + z * z + w * w;
	}

	// 
	// The normalized vector has a magnitude of 1 and is in the same direction as the current vector.
	// 
	// NOTE: this function does not modify the current vector. 
	// To modify and normalize the current vector, use the Normalize() function.
	// 
	// @return
	//		A normalized vector based on the current vector.
	// 
	_NODISCARD inline Vector4 normalized() const noexcept {
		return (sqrMagnitude() > 0) ? *this / magnitude() : *this;
	}

	// 
	// Makes this vector have a magnitude of 1.
	//		
	// When normalized, a vector keeps the same direction but its length is 1.0.
	// NOTE: This function will change the current vector. If you want to keep the current vector unchanged, use normalized().
	//		
	inline Vector4& Normalize() {
		Vector4 temp = *this;
		temp /= magnitude();
		return temp;
	}

	// 
	// Multiplies every component of this vector by the same component of _scale.
	// 
	// @return 
	//		A reference to this vector after applying scale.
	// 
	inline Vector4& Scale(const Vector4& _scale) noexcept {
		x *= _scale.x;
		y *= _scale.y;
		z *= _scale.z;
		w *= _scale.w;
		return *this;
	}

	// 
	// Multiplies every component of this vector by the same component of _scale.
	// 
	// @return 
	//		A new Vector4 contain values of the .
	// 
	inline Vector4 Scale(const Vector4& _scale) const noexcept {
		Vector4 temp = *this;
		temp.x *= _scale.x;
		temp.y *= _scale.y;
		temp.z *= _scale.z;
		temp.w *= _scale.w;
		return temp;
	}

	// 
	// Clamp every component of this vector to [0...1].
	// 
	inline Vector4& Clamp01() noexcept {
		x = LMK_Clamp(x, 0, 1);
		y = LMK_Clamp(y, 0, 1);
		z = LMK_Clamp(z, 0, 1);
		w = LMK_Clamp(w, 0, 1);
		return *this;
	}

public: // Static Functions
	// 
	// Get the direction from one Vector2 to another by using: (_to - _from).
	// 
	// @return
	//		A normalized Vector2 represents the calculated direction.
	// 
	_NODISCARD inline static Vector4 Direction(const Vector4& _from, const Vector4& _to) noexcept {
		return (_to - _from).normalized();
	}

	// 
	// Vector4.Distance(a,b) is the same as (a - b).magnitude().
	// 
	// @return
	//		The distance between _a and _b.
	// 
	_NODISCARD inline static float Distance(const Vector4& _a, const Vector4& _b) noexcept {
		return (_b - _a).magnitude();
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
	//		The result of (_lhs . _rhs).
	// 
	_NODISCARD inline static float Dot(const Vector4& _lhs, const Vector4& _rhs) noexcept {
		double result = 0;
		for (size_t i = 0; i < 4; i++) {
			result += _lhs[i] * _rhs[i];
		}
		return result;
	}

	// 
	// Returns a vector that is made from the largest components of two vectors.
	// 
	_NODISCARD inline static Vector4 Max(const Vector4& _a, const Vector4& _b) noexcept {
		return Vector4(
			LMK_Max(_a.x, _b.x),
			LMK_Max(_a.y, _b.y),
			LMK_Max(_a.z, _b.z),
			LMK_Max(_a.w, _b.w)
		);
	}

	// 
	// Returns a vector that is made from the smallest components of two vectors.
	// 
	_NODISCARD inline static Vector4 Min(const Vector4& _a, const Vector4& _b) noexcept {
		return Vector4(
			LMK_Min(_a.x, _b.x),
			LMK_Min(_a.y, _b.y),
			LMK_Min(_a.z, _b.z),
			LMK_Min(_a.w, _b.w)
		);
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
	_NODISCARD inline static Vector4 Lerp(const Vector4& _from, const Vector4& _to, float _t) noexcept {
		return LMK_Lerp(_from, _to, LMK_Clamp(_t, 0.0f, 1.0f));
	}

	// 
	// Linearly interpolates between vectors _a and _b by _t.
	// 
	// When _t = 0 returns _a.
	// When _t = 1 return _b.
	// When _t = 0.5 returns the midpoint of _a and _b.
	// 
	_NODISCARD inline static Vector4 LerpUnclamped(const Vector4& _from, const Vector4& _to, float _t) noexcept {
		return LMK_Lerp(_from, _to, _t);
	}

	// 
	// Moves a point current towards target.
	//		
	// This is essentially the same as Vector2::Lerp but instead the function will ensure 
	// that the distance never exceeds maxDistanceDelta. 
	//		
	// Negative values of _maxDistanceDelta pushes the vector away from target.
	// 
	_NODISCARD inline static Vector4 MoveTowards(const Vector4& _current, const Vector4& _target, float _maxDistanceDelta) noexcept {
		if (Distance(_current, _target) <= _maxDistanceDelta)
			return _target;
		return _current + (Direction(_current, _target) * _maxDistanceDelta);
	}

	// 
	// Multiplies two vectors component-wise.
	// 
	// Every component in the result is a component of _a multiplied by the same component of _b.
	// 
	_NODISCARD inline static Vector4 Scale(const Vector4& _a, const Vector4& _b) noexcept {
		return _a.Scale(_b);
	}

	//
	// Projects a vector onto another vector.
	// 
	// @return
	//		_a projected onto _b.
	// 
	_NODISCARD inline static Vector4 Project(const Vector4& _a, const Vector4& _b) {
		return _b * (Dot(_a, _b) / _b.sqrMagnitude());
	}

	//
	// Shorthand for writing Vector4(0, 0, 0, 0).
	//
	_NODISCARD inline static Vector4 zero() {
		return Vector4(0, 0, 0, 0);
	}

	//
	// Shorthand for writing Vector4(1, 1, 1, 1).
	//
	_NODISCARD inline static Vector4 one() {
		return Vector4(1, 1, 1, 1);
	}

	//
	// Shorthand for writing Vector4(INFINITY, INFINITY, INFINITY, INFINITY).
	//
	_NODISCARD inline static Vector4 positiveInfinity() {
		return Vector4(INFINITY, INFINITY, INFINITY, INFINITY);
	}

	//
	// Shorthand for writing Vector4(-INFINITY, -INFINITY, -INFINITY, -INFINITY).
	//
	_NODISCARD inline static Vector4 negativeInfinity() {
		return Vector4(-INFINITY, -INFINITY, -INFINITY, -INFINITY);
	}

public: // Properties
	base_type x;	// The X component of this vector.
	base_type y;	// The Y component of this vector.
	base_type z;	// The Z component of this vector.
	base_type w;	// The W component of this vector.
};

// Vector4:
// Non-member Operators Overloads

_NODISCARD inline Vector4 operator*(float _left, const Vector4& _right) {
	Vector4 temp = _right;
	temp *= _left;
	return temp;
}

// +--------------------------------------------------------------------------------+
// |																				|
// | MATRIX 3x3																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// A standard 3x3 transformation matrix used explicitly for 2D transformations.
//
class Matrix3 {
public: // Typedef
	// An lmk::BaseVector2 type.
	using vector_type	= Vector3;
	// An arithmetic type (i.e, int, float, double,...)
	using base_type		= typename vector_type::base_type;
	
	// The size of the matrix in either dimension.
	const static size_t size = 3;

public: // Constructors & Destructors
	//
	// Creates a new Matrix3 from an array of Vector3.
	// Each vector represents a row in the matrix.
	//
	inline Matrix3(const vector_type (&_matrix)[3]) {
		for (size_t i = 0; i < size; i++)
			for (size_t j = 0; j < size; j++)
				matrix[i][j] = _matrix[i][j];
	}

	//
	// Creates a new Matrix3 from an array of array.
	// Each vector represents a row in the matrix.
	//
	inline Matrix3(const base_type (&_matrix)[3][3]) {
		for (size_t i = 0; i < size; i++)
			for (size_t j = 0; j < size; j++)
				matrix[i][j] = _matrix[i][j];
	}

	//
	// Creates an identity Matrix3.
	//
	inline Matrix3()
		: matrix{
			{1, 0, 0},
			{0, 1, 0},
			{0, 0, 1}
			} {}

public: // Operators Overloads
	//
	// Returns a formatted string for this vector.
	//
	// @return
	//		A formatted string: [{m11; m12; m13}, {m21; m22; m23}, {m31; m32; m33}]
	//
	_NODISCARD inline operator std::string() const {
		std::string result;
		for (size_t i = 0; i < size; i++) {
			result += "[" + std::to_string(matrix[i][0]) + ", " + std::to_string(matrix[i][1]) + ", " + std::to_string(matrix[i][2]) + "]";
		}
		return result;
	}

	//
	// Access matrix at [_rowIndex], stored as a standard C array.
	//
	_NODISCARD inline base_type* operator[](int _rowIndex) {
		LMK_CORE_ASSERT(_rowIndex >= 0 && _rowIndex < size, "lmk::Matrix3: index out of range for operator[] (index should be 0, 1, or 2).");
		return matrix[_rowIndex];
	}

	//
	// Access matrix at [_rowIndex], stored as a ::vector_type.
	//
	_NODISCARD inline const base_type* operator[](int _rowIndex) const {
		LMK_CORE_ASSERT(_rowIndex >= 0 && _rowIndex < size, "lmk::Matrix3: index out of range for operator[] (index should be 0, 1, or 2).");
		return matrix[_rowIndex];
	}

	inline Matrix3& operator*=(const Matrix3& _right) noexcept {
		Matrix3 temp = *this;
		// Perform dot products for each row and column.
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				matrix[i][j] = vector_type::Dot(temp.GetRow(i), vector_type{_right[0][j], _right[1][j], _right[2][j]});
			}
		}
		return *this;
	}

	inline Matrix3& operator*=(const vector_type& _right) noexcept {
		Matrix3 temp = *this;
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				matrix[i][j] = vector_type::Dot(temp.GetRow(i), _right);
			}
		}
		return *this;
	}

	_NODISCARD inline Matrix3 operator*(const Matrix3& _right) const noexcept {
		Matrix3 temp = *this;
		temp *= _right;
		return temp;
	}

	_NODISCARD inline Matrix3 operator*(const vector_type& _right) const noexcept {
		Matrix3 temp = *this;
		temp *= _right;
		return temp;
	}

public: // Functions
	// 
	// Transforms a position by this matrix.
	//
	_NODISCARD inline Vector2 MultiplyPoint(Vector2 _vector) const noexcept {
		Vector2 result;
		result.x = matrix[0][0] * _vector.x + matrix[0][1] * _vector.y + matrix[0][2];
		result.y = matrix[1][0] * _vector.x + matrix[1][1] * _vector.y + matrix[1][2];
		return result;
	}

	//
	// Returns a column of the matrix.
	// The _index-th column is returned as a Vector3. (_index must be from 0 to 2 inclusive)
	//
	_NODISCARD inline vector_type GetColumn(int _index) const noexcept {
		return vector_type{ matrix[0][_index], matrix[1][_index], matrix[2][_index] };
	}

	//
	// Set a column of the matrix.
	// The _index-th column is set from Vector3. (_index must be from 0 to 2 inclusive)
	//
	_NODISCARD inline void SetColumn(int _index, const vector_type& _column) noexcept {
		for (size_t i = 0; i < size; i++) {
			matrix[i][_index] = _column[i];
		}
	}

	//
	// Returns a row of the matrix.
	// The _index-th row is returned as a Vector3. (_index must be from 0 to 2 inclusive)
	//
	_NODISCARD inline vector_type GetRow(int _index) const noexcept {
		return vector_type{ matrix[_index][0], matrix[_index][1], matrix[_index][2] };
	}

	//
	// Sets a row of the matrix.
	// The _index-th row is set from Vector3. (_index must be from 0 to 2 inclusive)
	//
	_NODISCARD inline void SetRow(int _index, const vector_type& _row) noexcept {
		for (size_t i = 0; i < size; i++) {
			matrix[_index][i] = _row[i];
		}
	}

	//
	// Sets this matrix to a translation, rotation and scaling matrix.
	// 
	// The current matrix is modified so that it places objects at position _t, oriented in rotation _r and scaled by _s.
	// 
	// @param _t:
	//		A Vector2 represents the 2D translation.
	// @param _r:
	//		A float represents the 2D rotation in degree.
	// @param _s:
	//		A Vector2 represents the 2D scale.
	//
	inline void SetTRS(Vector2 _t, float _r, Vector2 _s) {
		*this = Translate(_t) * Rotate(_r) * Scale(_s);
	}

public: // Static Functions
	//
	// Create an orthogonal projection matrix.
	// 
	// The returned matrix, when used as a Camera's projection matrix, creates a projection of the area between left, right, top and bottom
	// into a cube going from (left, bottom) = (-1, -1) to (right, top) = (1, 1).
	// 
	// @param _left:
	//		Left-side x-coordinate.
	// @param _right:
	//		Right-side x-coordinate.
	// @param _bottom:
	//		Bottom y-coordinate.
	// @param _top:
	//		Top y-coordinate.
	// 
	// @return 
	//		The projection matrix.
	//
	_NODISCARD inline static Matrix3 Ortho(base_type _left, base_type _right, base_type _bottom, base_type _top) {
		// The original formula for the construction of the projection matrix in 3D can be found here:
		// https://learnwebgl.brown37.net/08_projections/projections_ortho.html
		// 
		// This is a simplified version used specifically for 2D projection that removes the .

		// Calculating the elements of the matrix.
		float A = 2 / (_right - _left);						// The scaling factor in the x direction.
		float B = 2 / (_top - _bottom);						// The scaling factor in the y direction.
		float tx = -(_right + _left) / (_right - _left);	// The translation amount in the x direction.
		float ty = -(_top + _bottom) / (_top - _bottom);	// The translation amount in the y direction.

		// Constructing the matrix.
		return { {
			{A, 0, tx},
			{0, B, ty},
			{0, 0, 1}
			} };
	}

	//
	// Creates a rotation matrix.
	// 
	// @param _rotation:
	//		The rotation in degree.
	// 
	// @return
	//		The rotation matrix.
	//
	_NODISCARD inline static Matrix3 Rotate(float _rotation) {
		// Convert angle from degrees to radians.
		float angle_rad = LMK_DtoR(_rotation);

		// Calculate sin and cos of the angle.
		float cos_theta = std::cos(angle_rad);
		float sin_theta = std::sin(angle_rad);

		// Constructing the rotation matrix in homogeneous coordinates.
		return { {
			{cos_theta, -sin_theta, 0},
			{sin_theta, cos_theta, 0},
			{0, 0, 1}
		} };
	}

	//
	// Creates a scaling matrix.
	// 
	// @param _scale:
	//		A vector represents the scale factor in x and y coordinate.
	// 
	// @return
	//		The scaling matrix.
	//
	_NODISCARD inline static Matrix3 Scale(Vector2 _scale) {
		return { {
			{_scale.x, 0, 0},
			{0, _scale.y, 0},
			{0, 0, 1}
		} };
	}

	//
	// Creates a scaling matrix.
	// 
	// @param _scale:
	//		A vector represents the scale factor in x and y coordinate.
	// 
	// @return
	//		The scaling matrix.
	//
	_NODISCARD inline static Matrix3 Translate(Vector2 _translate) {
		return { {
			{1, 0, _translate.x},
			{0, 1, _translate.y},
			{0, 0, 1}
		} };
	}

	//
	// Creates a translation, rotation and scaling matrix.
	// 
	// The returned matrix is such that it places objects at position _t, oriented in rotation _r and scaled by _s.
	// 
	// @param _t:
	//		A Vector2 represents the 2D translation.
	// @param _r:
	//		A float represents the 2D rotation in degree.
	// @param _s:
	//		A Vector2 represents the 2D scale.
	//
	_NODISCARD inline static Matrix3 TRS(Vector2 _t, float _r, Vector2 _s) {
		return Translate(_t) * Rotate(_r) * Scale(_s);
	}

private: // Properties
	base_type matrix[3][3];
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
// where (0,0) represents the top left corner and Y increases downwards:
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
// @tparam Type:
//		Single coordinate type (e.g. int, float, ...).
// @tparam Type:
//		lmk::BaseVector2 type (e.g. Vector2Int, Vector2, ...).
//
template <typename Type>
class BaseRect {
protected: // Typedef
	// An lmk::BaseVector2 type.
	using vector_type	= typename Type;
	static_assert(is_lmkvector_t<vector_type>, "lmk::BaseRect: Invalid vector type.");
	// An arithmetic type (i.e, int, float, double,...)
	using base_type		= typename vector_type::base_type;

public: // Constructors & Destructors
	//
	// Creates a BaseRect<Type>.
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
	inline explicit BaseRect(base_type _x, base_type _y, base_type _w, base_type _h)
		: m_xMin(_x), m_yMin(_y), m_xMax(_x + _w), m_yMax(_y + _h), m_width(_w), m_height(_h) {}

	//
	// Creates a BaseRect<Type>.
	// 
	// @param _pos:
	//		The position (x, y) of the rectangle.
	// @param _size:
	//		The width (x) and height (y) of the rectangle.
	//
	inline explicit BaseRect(vector_type _pos, vector_type _size)
		: BaseRect(_pos.x, _pos.y, _size.x, _size.y) {}

public: // Operators Overloads
	//
	// Returns a formatted string for this rectangle.
	//
	// @return
	//		A formatted string: {{xMin; yMin} : {xMax; yMax}}
	// 
	_NODISCARD inline operator std::string() const {
		return "{" + (std::string)vector_type(m_xMin, m_yMin) + " : " + (std::string)vector_type(m_xMax, m_yMax) + "}";
	}

	template <typename Right_Type>
	_NODISCARD inline bool operator==(const BaseRect<Right_Type>& _right) const noexcept {
		return (m_xMin == _right.m_xMin) && (m_yMin == _right.m_yMin) && (m_width == _right.m_width) && (m_height == _right.m_height);
	}

public: // Functions
	//
	// Offset the position of the rectangle.
	//
	inline void Offset(vector_type _offset) noexcept {
		setPosition(position() + _offset);
	}

	//
	// Offset the position of the rectangle.
	//
	inline void Offset(base_type _x, base_type _y) noexcept {
		Offset(vector_type(_x, _y));
	}

	//
	// Scale width and height of the rectangle.
	//
	inline void Scale(vector_type _scale) noexcept {
		setWidth(m_width * _scale.x);
		setHeight(m_height * _scale.y);
	}

	//
	// Scale width and height of the rectangle.
	//
	inline void Scale(base_type _scaleX, base_type _scaleY) noexcept {
		Scale(vector_type(_scaleX, _scaleY));
	}

	//
	// Scale width and height of the rectangle.
	//
	inline void Scale(base_type _scale) noexcept {
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
	_NODISCARD inline bool Contains(vector_type _point, bool _allowInverse = false) const noexcept {
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
	// @tparam Right_Type: 
	//		The <Type> of the other rectangle.
	// 
	template <typename Right_Type>
	_NODISCARD inline bool Overlaps(const BaseRect<Right_Type>& _other) const noexcept {
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
	_NODISCARD inline base_type area() const noexcept {
		return m_width * m_height;
		return m_width * m_height;
	}

public: // Accessors
	// Returns the width of the rectangle, measured from the X position.
	_NODISCARD inline base_type width() const noexcept {
		return m_width;
	}

	// Returns the height of the rectangle, measured from the Y position.
	_NODISCARD inline base_type height() const noexcept {
		return m_height;
	}

	// Get the width and height of the rectangle.
	_NODISCARD inline vector_type size() const noexcept {
		return vector_type(m_width, m_height);
	}

	// Get the minimum X coordinate of the rectangle. 
	_NODISCARD inline base_type xMin() const noexcept {
		return m_xMin;
	}

	// Get the minimum Y coordinate of the rectangle. 
	_NODISCARD inline base_type yMin() const noexcept {
		return m_yMin;
	}

	// Get the position of the minimum corner of the rectangle.
	_NODISCARD inline vector_type minPos() const noexcept {
		return vector_type(m_xMin, m_yMin);
	}

	// Get the maximum X coordinate of the rectangle. 
	_NODISCARD inline base_type xMax() const noexcept {
		return m_xMax;
	}

	// Get the maximum Y coordinate of the rectangle. 
	_NODISCARD inline base_type yMax() const noexcept {
		return m_yMax;
	}

	// Get the position of the maximum corner of the rectangle.
	_NODISCARD inline vector_type maxPos() const noexcept {
		return vector_type(m_xMax, m_yMax);
	}

	// Get the X and Y position of the rectangle.
	_NODISCARD inline vector_type position() const noexcept {
		return vector_type(m_xMin, m_yMin);
	}

public: // Mutators
	// Set the width of the rectangle, measured from the X position.
	// Setting this property will preserve the Min coordinate but changes and Max coordinate.
	inline void setWidth(base_type _width) noexcept {
		m_width = _width;
		m_xMax = m_xMin + m_width;
	}

	// Set the height of the rectangle, measured from the Y position.
	// Setting this property will preserve the Min coordinate but changes and Max coordinate.
	inline void setHeight(base_type _height) noexcept {
		m_height = _height;
		m_yMax = m_yMin + m_height;
	}

	// Set the width and height of the rectangle.
	// Setting this property will preserve the Min coordinate but changes and Max coordinate.
	inline void setSize(base_type _width, base_type _height) noexcept {
		m_width = _width;
		m_height = _height;
		m_xMax = m_xMin + m_width;
		m_yMax = m_yMin + m_height;
	}

	// Set the width and height of the rectangle.
	// Setting this property will preserve the Min coordinate but changes and Max coordinate.
	inline void setSize(vector_type _size) noexcept {
		setSize(_size.x, _size.y);
	}

	// Set the minimum X coordinate of the rectangle. 
	// Setting this property will resize the width of the rectangle.
	inline void setXMin(base_type _xMin) noexcept {
		m_xMin = _xMin;
		m_width = m_xMax - m_xMin;
	}

	// Set the minimum X coordinate of the rectangle. 
	// Setting this property will resize the height of the rectangle.
	inline void setYMin(base_type _yMin) noexcept {
		m_yMin = _yMin;
		m_height = m_yMax - m_yMin;
	}

	// Set the position of the minimum corner of the rectangle.
	// Setting this property will resize the rectangle and preserve the position of the Max coordinate.
	inline void setMin(base_type _x, base_type _y) noexcept {
		m_xMin = _x;
		m_yMin = _y;
		m_width = m_xMax - m_xMin;
		m_height = m_yMax - m_yMin;
	}

	// Set the position of the minimum corner of the rectangle.
	// Setting this property will resize the rectangle and preserve the position of the Max coordinate.
	inline void setMin(vector_type _minPos) noexcept {
		setMin(_minPos.x, _minPos.y);
	}

	// Set the maximum X coordinate of the rectangle. 
	// Setting this property will resize the width of the rectangle.
	inline void setXMax(base_type _xMax) noexcept {
		m_xMax = _xMax;
		m_width = m_xMax - m_xMin;
	}

	// Set the maximum Y coordinate of the rectangle. 
	// Setting this property will resize the height of the rectangle.
	inline void setYMax(base_type _yMax) noexcept {
		m_yMax = _yMax;
		m_height = m_yMax - m_yMin;
	}

	// Set the position of the maximum corner of the rectangle.
	// Setting this property will resize the rectangle and preserve the position of the Min coordinate.
	inline void setMax(base_type _x, base_type _y) noexcept {
		m_xMax = _x;
		m_yMax = _y;
		m_width = m_xMax - m_xMin;
		m_height = m_yMax - m_yMin;
	}

	// Set the position of the maximum corner of the rectangle.
	// Setting this property will resize the rectangle and preserve the position of the Min coordinate.
	inline void setMax(vector_type _maxPos) noexcept {
		setMax(_maxPos.x, _maxPos.y);
	}

	// Set the X and Y position of the rectangle. 
	// Setting this property will preserve the size of the rectangle but changes the Min and Max coordinate.
	inline void setPosition(base_type _x, base_type _y) noexcept {
		m_xMin = _x;
		m_yMin = _y;
		m_xMax = _x + m_width;
		m_yMax = _y + m_height;
	}

	// Set the X and Y position of the rectangle.
	// Setting this property will preserve the size of the rectangle but changes the Min and Max coordinate.
	inline void setPosition(vector_type _pos) noexcept {
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
	base_type m_xMin, m_yMin;
	// The position of the maximum corner of the rectangle.
	base_type m_xMax, m_yMax;

	base_type m_width, m_height;
};
IMPL_END

//
// A 2D Rectangle defined by X and Y position, width and height.
// 
class Rect : public impl::BaseRect<Vector2> {
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
	inline explicit Rect(base_type _x, base_type _y, base_type _w, base_type _h)
		: BaseRect(_x, _y, _w, _h) {}

	//
	// Creates a new Rect.
	// 
	// @param _pos:
	//		The position (x, y) of the rectangle.
	// @param _size:
	//		The width (x) and height (y) of the rectangle.
	//
	inline explicit Rect(vector_type _position, vector_type _size)
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
	_NODISCARD inline static vector_type NormalizedToPoint(const Rect& _rect, vector_type _normRectCoord) {
		return vector_type::Scale(_normRectCoord, _rect.size());
	}

	//
	// Get the normalized coordinates cooresponding to the given point.
	// 
	// @return
	//		A Vector2 in the range 0 to 1 with values more 1 or less than zero clamped.
	//
	_NODISCARD inline static vector_type PointToNormalized(const Rect& _rect, vector_type _point) {
		return Vector2(
			(_point.x - _rect.xMin()) / _rect.width(),
			(_point.y - _rect.yMin()) / _rect.height()
		).Clamp01();
	}

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
class RectInt : public impl::BaseRect<Vector2Int> {
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
	inline explicit RectInt(base_type _x, base_type _y, base_type _w, base_type _h)
		: BaseRect(_x, _y, _w, _h) {}

	//
	// Creates a new Rect.
	// 
	// @param _pos:
	//		The position (x, y) of the RectInt.
	// @param _size:
	//		The width (x) and height (y) of the RectInt.
	//
	inline explicit RectInt(vector_type _position, vector_type _size)
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
	inline Ray(Vector2 _origin, Vector2 _direction)
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
	inline explicit Bounds(Vector2 _center, Vector2 _size)
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
	inline void Expand(Vector2 _amount) {
		setSize(m_size + _amount);
	}

	//
	// Grows the Bounds to include the _point.
	// 
	// This function will find the minimum extents 
	//
	inline void Encapsulate(Vector2 _point) {
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
	// @return
	//		A Vector2 represents the point on the bounding box or inside the bounding box.
	//
	_NODISCARD inline Vector2 ClosestPoint(Vector2 _point) const noexcept {
		return Vector2(
			LMK_Clamp(_point.x, m_min.x, m_max.x),
			LMK_Clamp(_point.y, m_min.y, m_max.y)
		);
	}

	//
	// Is _point contained in the bounding box?
	//
	_NODISCARD inline bool Contains(Vector2 _point) const noexcept {
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
	inline void setCenter(Vector2 _center) {
		m_center = _center;
		m_min = m_center - m_extents;
		m_max = m_center + m_extents;
	}

	// Set the center of the bounding box. This is always twice as large as the extents.
	inline void setSize(Vector2 _size) {
		m_size = _size;
		m_extents = m_size / 2;
		m_min = m_center - m_extents;
		m_max = m_center + m_extents;
	}

	// Set the extents of the Bounding Box. This is always half of the size of the Bounds.
	inline void setExtents(Vector2 _extents) {
		m_extents = _extents;
		m_size = m_extents * 2;
		m_min = m_center - m_extents;
		m_max = m_center + m_extents;
	}

	// Set the minimal point of the box. This is always equal to center - extents.
	inline void setMin(Vector2 _min) {
		m_min = _min;
		m_size = m_max - m_min;
		m_extents = m_size / 2;
		m_center = m_min + m_extents;
	}

	// Set the maximal point of the box. This is always equal to center + extents.
	inline void setMax(Vector2 _max) {
		m_max = _max;
		m_size = m_max - m_min;
		m_extents = m_size / 2;
		m_center = m_max - m_extents;
	}

	// Sets the bounds to the min and max value of the box.
	// Using this function is faster than assigning min and max separately.
	inline void setMinMax(Vector2 _min, Vector2 _max) {
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
	// Create a new Color.
	// 
	// @param _r:
	//		The red component of the color.
	// @param _g:
	//		The green component of the color.
	// @param _b:
	//		The blue component of the color.
	// @param _a:
	//		Alpha component of the color (0 is transparent, 1 is opaque).
	//
	inline Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) 
		: r(_r), g(_g), b(_b), a(_a) {}

	//
	// Creates a completely transparent black Color (0, 0, 0, 0).
	//
	inline Color() 
		: Color(0, 0, 0, 0) {}

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
		switch (_index) {
		case 0: return r;
		case 1: return g;
		case 2: return b;
		case 3: return a;
		}
		LMK_CORE_ASSERT(false, "lmk::Color: index out of range for operator[] (index should be 0, 1, 2 or 3).");
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

		switch (hi) {
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
	uint8_t r;	// Red component of the color.
	uint8_t g;	// Green component of the color.
	uint8_t b;	// Blue component of the color.
	uint8_t a;	// Alpha component of the color (0 is transparent, 1 is opaque).
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
protected:
	inline Object() = default;

	inline virtual ~Object() = default;

public: // Operators Overloads
	//
	// Does this object exist?
	//
	_NODISCARD inline operator bool() {
		return this != nullptr;
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
	// NOTE: When destroying MonoBehaviour scripts, the engine calls OnDisable and OnDestroy before the script is removed.
	// 
	// @param _obj:
	//		The object to destroy.
	// @param _t:
	//		The optional amount of time to delay before destroying the object.
	//
	inline static void Destroy(Object* _obj, float _t = 0.0F) {
		std::thread destroy(DestroyDelay, _obj, _t);

		destroy.join();
	}

private:
	inline static void DestroyDelay(Object* _obj, float _t = 0.0F) {
		std::this_thread::sleep_for(std::chrono::duration<double>(_t));
		delete _obj;
	}

public: // Properties
	std::string name;	// The name of the object.

private:
	// NOTE:
	// Currently, this property serve no purpose in the functionality of the Object class.
	// This is a placeholder for future implementation of file system and prefab.
	int m_id;
};
LMK_END

#endif // !LMK_COREMDL_2D_H_