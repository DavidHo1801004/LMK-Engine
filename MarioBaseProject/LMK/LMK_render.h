#ifndef LMK_RENDER_H_
#define LMK_RENDER_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL
#include "LMK_coremdl.h"

LMK_BEGIN
class RenderUtil {
public:
	//
	// For detailed explaination for the following formula, see:
	// https://inginious.org/course/competitive-programming/geometry-rotation
	//
	inline static Vector2 RotatePointAround(const Vector2& _point, const Vector2& _pivot, float _angle) {
		return Vector2(_pivot.x + (_point.x - _pivot.x) * std::cos(_angle) - (_point.y - _pivot.y) * std::sin(_angle),
					   _pivot.y + (_point.x - _pivot.x) * std::sin(_angle) + (_point.y - _pivot.y) * std::cos(_angle));
	}
};
LMK_END

#endif // LMK_HAVE_SDL
#endif // !LMK_RENDER_H_