#ifndef LMK_RENDER_H_
#define LMK_RENDER_H_

#include "LMK_stdinc.h"
#if LMK_HAVE_SDL
#include "LMK_coremdl.h"

LMK_BEGIN
class RenderUtil {
public:
	inline static Vector2Int GetRenderCoordFromFoat(const Vector2Int& _res, const Vector2& _floatPt) {


		return _res;
	}
};
LMK_END

#endif // LMK_HAVE_SDL
#endif // !LMK_RENDER_H_