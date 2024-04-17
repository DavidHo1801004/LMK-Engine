#ifndef LMK_COREMDL_H_
#define LMK_COREMDL_H_

#include "LMK_stdinc.h"

LMK_BEGIN
// +--------------------------------------------------------------------------------+
// | 2D VECTOR																		|
// +--------------------------------------------------------------------------------+
#pragma warning(disable : 4244)
IMPL_BEGIN
//
// The base class for Vector2 and Vector2Int.
//
template<typename coord_type>
class BaseVector2 {
public:	// Typedef
	using coord_t = coord_type;

public:	// Constructors & Destructors
	//
	// Creates a new BaseVector2<coord_type>.
	// 
	// @param _x:
	//		The X coordinate of the vector.
	// @param _y:
	//		The Y coordinate of the vector.
	//
	inline BaseVector2(coord_t _x, coord_t _y) : x(_x), y(_y) {}

	//
	// Creates a new BaseVector2<coord_type> with coordinate of (0, 0).
	//
	inline BaseVector2() : BaseVector2(0, 0) {}

public:	// Operator overloads
	//
	// Returns a formatted string for this BaseVector2.
	//
	// @return
	//		A formatted string: (x; y)
	//
	_NODISCARD inline operator std::string() {
		return "(" + std::to_string(x) + "; " + std::to_string(y) + ")";
	}

	//
	// Access the x or y component using [0] or [1] respectively.
	//
	_NODISCARD constexpr coord_t operator[](int _index) noexcept {
		switch (_index) {
		case 0:
			return x;
		case 1:
			return y;
		default:
			return 0;	
			// I have no idea what to do here.
			// Returning 0 is just a temporary solution.
		}
	}

	template<typename right_coord_t>
	_NODISCARD constexpr bool operator==(BaseVector2<right_coord_t> _right) const noexcept {
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
	explicit inline Vector2(coord_t _x, coord_t _y)	: BaseVector2(_x, _y) {}

	//
	// Creates a new Vector2 with coordinate of (0, 0).
	//
	explicit inline Vector2() : BaseVector2() {}

public: // Operator overloads
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
	inline void Normalize() noexcept {
		x /= magnitude();
		y /= magnitude();
	}

	// 
	// Multiplies every component of this vector by the same component of _scale.
	// 
	_NODISCARD inline void Scale(Vector2 _scale) {
		x *= _scale.x;
		y *= _scale.y;
	}

public: // Static Functions
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
	// Vector2.Distance(a,b) is the same as (a - b).magnitude().
	// 
	// @return
	//		The distance between _a and _b.
	// 
	_NODISCARD inline static float Distance(Vector2 _a, Vector2 _b) noexcept {
		return (_b - _a).magnitude();
	}

	// 
	// Clamp tthe magnitude of a Vector2.
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
	_NODISCARD inline static float Angle(Vector2 _from, Vector2 _to) {
		return std::abs(SignedAngle(_from, _to));
	}

	// 
	// Gets the unsigned angle in degrees between _from and _to.
	//	
	// The angle returned is the unsigned angle between the two vectors.
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
	_NODISCARD inline static float SignedAngle(Vector2 _from, Vector2 _to) {
		return LMK_RtoD(std::atan2(_from.x * _to.y - _from.y * _to.x, _from.x * _to.x + _from.y * _to.y));
	}

	// 
	// Returns a vector that is made from the largest components of two vectors.
	// 
	// @return
	//		A copy of the larger Vector2.
	// 
	_NODISCARD inline static Vector2 Max(Vector2 _a, Vector2 _b) noexcept {
		return (_a.sqrMagnitude() > _b.sqrMagnitude()) ? _a : _b;
	}

	// 
	// Returns a vector that is made from the smallest components of two vectors.
	// 
	// @return
	//		A copy of the smaller Vector2.
	// 
	_NODISCARD inline static Vector2 Min(Vector2 _a, Vector2 _b) noexcept {
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
	_NODISCARD inline static Vector2 Lerp(Vector2 _from, Vector2 _to, float _t) noexcept {
		return _from + ((_to - _from) * LMK_Clamp(_t, 0.0f, 1.0f));
	}

	// 
	// Linearly interpolates between vectors _a and _b by _t.
	// 
	// When _t = 0 returns _a.
	// When _t = 1 return _b.
	// When _t = 0.5 returns the midpoint of _a and _b.
	// 
	_NODISCARD inline static Vector2 LerpUnclamped(Vector2 _from, Vector2 _to, float _t) noexcept {
		return _from + ((_to - _from) * _t);
	}

	// 
	// Moves a point current towards target.
	//		
	// This is essentially the same as Vector2.Lerp but instead the function will ensure 
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
	_NODISCARD inline static Vector2 Reflect(Vector2 _inDirection, Vector2 _inNormal) {
		return _inDirection - (_inNormal * 2) * Dot(_inDirection, _inNormal);
	}

	// 
	// Multiplies two vectors component-wise.
	// 
	// Every component in the result is a component of _a multiplied by the same component of _b.
	// 
	_NODISCARD inline static Vector2 Scale(Vector2 _a, Vector2 _b) {
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
	_NODISCARD inline static Vector2 SmoothDamp(Vector2 _current, Vector2 _target, Vector2& _currentVelocity,
		float _smoothTime, float _maxSpeed = INFINITY) {

		float deltaTime = 0;
		//========================= PENDING =========================//
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
	explicit inline Vector2Int(coord_t _x, coord_t _y) : BaseVector2(_x, _y) {}

	//
	// Creates a new Vector2Int with coordinate of (0, 0).
	//
	explicit inline Vector2Int() : BaseVector2() {}

public: // Operator overloads
	// 
	// Converts a Vector2Int to Vector2.
	// 
	inline operator Vector2() noexcept {
		return Vector2(x, y);
	}

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
	inline void Clamp(Vector2Int _min, Vector2Int _max) noexcept {
		x = LMK_Clamp(x, _min.x, _max.x);
		y = LMK_Clamp(y, _min.y, _max.y);
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

	// 
	// Vector2.Distance(a,b) is the same as (a - b).magnitude().
	// 
	// @return
	//		The distance between _a and _b.
	// 
	_NODISCARD inline static float Distance(Vector2Int _from, Vector2Int _to) noexcept {
		return (_to - _from).magnitude();
	}

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
		coord_t x = _a.x * _b.x;
		coord_t y = _a.y * _b.y;
		return Vector2Int(x, y);
	}
};

// +--------------------------------------------------------------------------------+
// | RECT																			|
// +--------------------------------------------------------------------------------+
//
// LMK Engine uses a coordinate system similar to that of Window Application, 
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
// 
//
template<typename coord_type>
class BaseRect {
public: // Typedef
	using coord_t	= coord_type;
	using Vector2_t	= BaseVector2<coord_t>;

public: // Constructors & Destructors
	//
	// Creates a BaseRect<coord_type>.
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
	inline BaseRect(coord_t _x, coord_t _y, coord_t _w, coord_t _h) 
		: xMin(_x), yMin(_y), xMax(_x + _w), yMax(_y + _h), w(_w), h(_h) {}

	//
	// Creates a BaseRect<coord_type>.
	// 
	// @param _pos:
	//		The position (x, y) of the rectangle.
	// @param _size:
	//		The width (x) and height (y) of the rectangle.
	//
	inline BaseRect(Vector2_t _pos, Vector2_t _size) : BaseRect(_pos.x, _pos.y, _size.x, _size.y) {}

public: // Operators
	//
	// Returns the virtual center as a Vector2.
	// 
	_NODISCARD inline operator std::string() {
		return (std::string)Vector2_t(xMin, yMin) + " : " + (std::string)Vector2_t(xMax, yMax);
	}

	template<typename right_coord_t>
	_NODISCARD inline bool operator==(const BaseRect<right_coord_t>& _right) const noexcept {
		return (xMin == _right.xMin) && (yMin == _right.yMin) && (w == _right.w) && (h == _right.h);
	}

public: // Functions
	//
	// Offset the position of the rectangle and reserve its size.
	// 
	// @param _x:
	//		The offset amount in the X axis;
	// @param _y:
	//		The offset amount in the Y axis;
	//
	inline void Offset(coord_t _x, coord_t _y) noexcept {
		xMin += _x;
		xMax += _x;
		yMin += _y;
		yMax += _y;
	}
	//
	// Offset the position of the rectangle and reserve its size.
	// 
	// @param _offset:
	//		The offset amount.
	//
	inline void Offset(Vector2_t _offset) noexcept {
		xMin += _offset.x;
		xMax += _offset.x;
		yMin += _offset.y;
		yMax += _offset.y;
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
	_NODISCARD inline bool Contains(Vector2_t _point, bool _allowInverse = false) const noexcept {
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
	template<typename right_coord_t>
	_NODISCARD inline bool Overlaps(BaseRect<right_coord_t> _other) const noexcept {
		// If either rectangle has area of 0 -> no overlap possible.
		if (area() == 0 || _other.area() == 0)
			return false;

		// If one rectangle is on left side of the other.
		if (xMin > _other.xMax || _other.xMin > xMax)
			return false;

		// If one rectangle is above the other.
		if (yMin > _other.yMax || _other.yMin > yMax)
			return false;

		return true;
	}

protected:
	// 
	// Get the area of this Rect.
	// 
	_NODISCARD inline coord_t area() const noexcept {
		return w * h;
	}

public: // Property Modifiers
	inline void setMin(coord_t _x, coord_t _y) noexcept {
		xMin = _x;
		yMin = _y;
	}
	inline void setMin(Vector2_t _minPos) noexcept {
		xMin = _minPos.x;
		yMin = _minPos.y;
	}

	inline void setMax(coord_t _x, coord_t _y) noexcept {
		xMax = _x;
		yMax = _y;
	}
	inline void setMax(Vector2_t _maxPos) noexcept {
		xMax = _maxPos;
		yMax = _maxPos;
	}

	inline void setPosition(Vector2_t _newPos) noexcept {
		xMin = _newPos.x;
		yMin = _newPos.y;
	}
	inline void setPosition(coord_t _x, coord_t _y) noexcept {
		xMin = _x;
		yMin = _y;
	}

	_NODISCARD inline coord_t getWidth() const noexcept {
		return w;
	}
	inline void setWidth(coord_t _width) noexcept {
		w = _width;
		xMax = xMin + w;
	}
 
	_NODISCARD inline coord_t getHeight() const noexcept {
		return h;
	}
	inline void setHeight(coord_t _height) noexcept {
		h = _height;
		yMax = yMin + h;
	}

protected: // Properties
	coord_t xMin, xMax;
	coord_t yMin, yMax;

	coord_t w, h;

	// The m_position of the center of the rectangle.
	Vector2 m_center;
};
IMPL_END

//
// A 2D Rectangle defined by X and Y m_position, width and height.
// 
class Rect : public impl::BaseRect<float> {
public:
	using coord_t = BaseRect::coord_t;

public:	// Constructors & Destructors
	//
	// 
	//
	inline Rect(coord_t _x, coord_t _y, coord_t _w, coord_t _h) : BaseRect(_x, _y, _w, _h) {}

	//
	// 
	//
	inline Rect(Vector2 _position, Vector2 _size) : Rect(_position.x, _position.y, _size.x, _size.y) {}

	//
	// 
	//
	inline Rect() : Rect(0, 0, 0, 0) {}

public: // Operator overloads
	_NODISCARD inline bool operator==(const Rect& _right) const noexcept {
		return (m_position == _right.m_position) && (m_size == _right.m_size);
	}

public: // Static Functions
	// 
	// Shorthand for writing Rect(0, 0, 0, 0).
	// 
	_NODISCARD inline static Rect zero() {
		return Rect(0, 0, 0, 0);
	}

	//
	// 
	//
	_NODISCARD static Rect MinMaxRect(coord_t xmin, coord_t ymin, coord_t xmax, coord_t ymax) {}

	//
	// 
	//
	_NODISCARD static Vector2 NormalizedToPoint(Rect rectangle, Vector2 normalizedRectCoordinates) {}

public: // Property Modifiers


private:
	// The X and Y position of the rectangle.
	Vector2 m_position;
	// The width and height of the rectangle.
	Vector2 m_size;
};

class RectInt : public impl::BaseRect<int> {
public: // Typedef
	using coord_t = BaseRect::coord_t;

public:	// Constructors & Destructors
	//
	// 
	//
	inline RectInt(coord_t _x, coord_t _y, coord_t _w, coord_t _h) : BaseRect(_x, _y, _w, _h) {}

	//
	// 
	//
	inline RectInt(Vector2Int _position, Vector2Int _size) : RectInt(_position.x ,_position.y, _size.x, _size.y) {}

	//
	// 
	//
	inline RectInt() : RectInt(0, 0, 0, 0) {}

public: // Operator overloads


public: // Functions


public: // Static Functions
	// 
	// Shorthand for writing RectInt(0, 0, 0, 0).
	// 
	_NODISCARD inline static RectInt zero() {
		return RectInt(0, 0, 0, 0);
	}

public: // Property Modifiers


public:	// Properties
	// Coordinate of the top left point of the Rect.
	Vector2Int m_position;
	// The width and height of the rectangle.
	Vector2Int size;
};
#pragma warning(default : 4244)

// +--------------------------------------------------------------------------------+
// | TRANSFORM			 															|
// +--------------------------------------------------------------------------------+

//
// Position, rotation and scale of an object.
//
class Transform {
public: // Constructors & Destructors
	inline Transform() {
		
	}

public:	// Functions
	inline void SetAsFirstSibling() {}
	inline void SetAsLastSibling() {}

	inline void DetachChildren() {}

	_NODISCARD inline bool IsChildOf() const {}

	_NODISCARD inline Transform* Find(std::string _name) const {}

	_NODISCARD inline Transform* GetChild(int _index) const {}

	_NODISCARD inline Vector2 TransformDirection(Vector2 _direction) const {}
	_NODISCARD inline std::vector<Vector2> TransformDirections(std::vector<Vector2> _directions) const {}

	_NODISCARD inline Vector2 TransformPoint(Vector2 _point) const {}
	_NODISCARD inline std::vector<Vector2> TransformPoints(std::vector<Vector2> _points) const {}

	_NODISCARD inline Vector2 InverseTransformDirection(Vector2 _direction) const {}
	_NODISCARD inline std::vector<Vector2> InverseTransformDirections(std::vector<Vector2> _directions) const {}

	_NODISCARD inline Vector2 InverseTransformPoint(Vector2 _point) const {}
	_NODISCARD inline std::vector<Vector2> InverseTransformPoints(std::vector<Vector2> _points) const {}

	_NODISCARD inline void Rotate(float _angle, bool _worldSpace = false) {}

	_NODISCARD inline void RotateAround(Vector2 _point, float _angle) {}

	_NODISCARD inline void Translate(Vector2 _translation) {}
	_NODISCARD inline void Translate(Vector2 _translation, bool _worldSpace = false) {}
	_NODISCARD inline void Translate(Vector2 _translation, const Transform& _relativeTo) {}

public: // Accesors & Mutators
	_NODISCARD inline uint8_t childCount() const {}

	_NODISCARD inline Vector2 up() const {}

	_NODISCARD inline Vector2 right() const {}

	_NODISCARD inline Transform* root() {}

	_NODISCARD inline Vector2 getPosition() const {}
	inline void setPosition(Vector2 _newVal) {}

	_NODISCARD inline Vector2 getLocalPosition() const {}
	inline void setLocalPosition(Vector2 _newVal) {}

	_NODISCARD inline float getRotation() const {}
	inline void setRotation(float _newVal) {}

	_NODISCARD inline float getLocalRotation() const {}
	inline void setLocalRotation(float _newVal) {}

	_NODISCARD inline Vector2 getLossyScale() {}

	_NODISCARD inline Vector2 getLocalScale() const {}
	inline void setLocalScale(Vector2 _newVal) {}

	_NODISCARD inline int getSiblingIndex() const {}
	inline void setSiblingIndex(int _index) {}

	_NODISCARD inline Transform* getParent() const {}
	inline void setParent(Transform* _p) {}
	inline void setParent(Transform* _parent, bool _worldPositionStays) {}

protected:	// Properies
	Vector2 m_lossyScale;
	Vector2 m_localscale	= Vector2::one();

	Vector2 m_position;
	Vector2 m_localPosition	= Vector2::zero();

	float m_rotation;
	float m_localrotation	= 0;

	bool m_isRoot;
	int m_siblingIndex;
	Transform* m_parent		= nullptr;
};

// +--------------------------------------------------------------------------------+
// | RECT TRANSFORM																	|
// +--------------------------------------------------------------------------------+

class RectTransform : public Transform {
public:

};

// +--------------------------------------------------------------------------------+
// | GAME OBJECT																	|
// +--------------------------------------------------------------------------------+

class GameObject {
public:
	GameObject() {}

public:
	RectTransform* transform	= nullptr;
};
LMK_END

#endif // !LMK_COREMDL_2D_H_