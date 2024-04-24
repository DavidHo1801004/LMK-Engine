#pragma once
#ifndef LMK_PHYSICS_2D_H_
#define LMK_PHYSICS_2D_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"
#include "LMK_systems.h"

LMK_BEGIN
//
//
//
enum class PhysicsShapeType2D {
	Circle,
	Capsule,
	Polygon,
	Edges,
};

//
//
//
struct PhysicsShape2D {
	float radius;
	PhysicsShapeType2D shapeType;
	uint32_t vertexCount;

private:
	std::vector<Vector2> vertices;
};

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
class Physics2D {
public: // Functions
	inline static RaycastHit2D Raycast(Vector2 origin, Vector2 direction, float distance = INFINITY, int layerMask, float minDepth = -INFINITY, float maxDepth = INFINITY) {

	}

public: // Properties
	static float gravitationalAcceleration;
};
LMK_END

#endif // !LMK_PHYSICS_2D_H_