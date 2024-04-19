#ifndef LMK_GISMO_H_
#define LMK_GISMO_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL
#include "LMK_coremdl.h"

LMK_BEGIN
class Gizmo {
public:
	//
	//
	//
	inline static void DrawLine(Vector2 _from, Vector2 _to) {

	}

	//
	// 
	//
	inline static void DrawLine(Vector2 _from, Vector2 _direction, float _distance) {

	}

	inline static void DrawRect() {

	}

	inline static void DrawFillRect() {

	}

	inline static void DrawCircle() {

	}

	inline static void DrawFillCircle() {

	}
};
LMK_END

#endif // LMK_HAVE_SDL
#endif // !LMK_GISMO_H_