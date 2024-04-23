#ifndef LMK_PHYSICS_2D_H_
#define LMK_PHYSICS_2D_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"
#include "LMK_systems.h"

LMK_BEGIN
//
//
//
class Collider2D {

};

//
// 
//
struct RaycastHit2D {
	Collider2D collider;

};

//
//
//
class Physics {
public: // Functions
	inline static RaycastHit2D Raycast(Vector2 origin, Vector2 direction, float distance = INFINITY, int layerMask, float minDepth = -INFINITY, float maxDepth = INFINITY) {

	}

public: // Properties
	static float gravitationalAcceleration;
};
LMK_END

#endif // !LMK_PHYSICS_2D_H_