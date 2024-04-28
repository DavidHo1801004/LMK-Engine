#pragma once
#ifndef LMK_PHYSICS_2D_H_
#define LMK_PHYSICS_2D_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"
#include "LMK_systems.h"

LMK_BEGIN
// +--------------------------------------------------------------------------------+
// |																				|
// | PHYSICS SHAPE 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Options for indicate which primitive shape type is used to interpret geometry 
// contained within a PhysicsShape2D object.
//
enum class PhysicsShapeType2D {
	// Use a circle shape to interpret the PhysicsShape2D geometry.
	// 
	// A circle geometry type is comprised of a single vertex in the PhysicsShape2D and a PhysicsShape2D.radius.
	// The single vertex represents the center of the circle with the radius defining the radius of the circle.
	Circle,

	// Use a capsule shape to interpret the PhysicsShape2D geometry.
	//
	// A capsule geometry type is comprised of two vertices in the PhysicsShape2D and a PhysicsShape2D.radius.
	// The two vertices represent the centers of the two ends of a capsule shape with the radius defining the circular end radius.
	Capsule,

	// Use a convex polygon shape to interpret the PhysicsShape2D geometry.
	//
	// A polygon geometry type is comprised of triangles.
	// The vertices represent a set of consecutive edges where each vertex connects to the next vertex
	// with the last vertex automatically representing an edge connecting to the first vertex resulting in a closed primitive shape.
	// 
	// These edges must result in a convex polygon.
	// If this is not the case then the physics system will constrain the polygon to be convex by producing a convex hull.
	// All polygon collision detection uses SAT (Seperating Axis Theorem) convex polygons for performance reasons.
	Polygon,
};

//
// Represents an efficient low-level physics shape used by the physics engine.
//
struct PhysicsShape2D {
	// The shape type determines how the vertices and radius are used by this PhysicsShape2D.
	PhysicsShapeType2D shapeType;

	// All PhysicsShapeType2D use this radius with the exception of the Polygon type.
	float radius;

	// Coordinate of vertices relative to the origin (0, 0) of the shape.
	// Scales and rotation transformation are based around this original pivot point.
	std::vector<Vector2> vert;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | BASE COLLIDER 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

struct RaycastHit2D;

//
// Base class for collider types used with 2D physics.
//
class BaseCollider2D {
public:
	inline BaseCollider2D(const PhysicsShape2D& _phyShape) 
		: m_pS(_phyShape), tvert(_phyShape.vert) {}

	inline BaseCollider2D() {}

public:
	//
	// 
	//
	_NODISCARD inline int Cast(Vector2 _direction, std::vector<RaycastHit2D>& _results, float _distance = INFINITY, bool _ignoreSiblingColliders = true) {

	}

	//
	// 
	//
	_NODISCARD inline Vector2 ClosetPoint(Vector2 _position) {

	}

	//
	// Checking for overlapping colliders using SAT (Seperating Axis Theorem).
	//
	_NODISCARD inline bool OverlapWith(const BaseCollider2D& _other) {
		const BaseCollider2D* collider1 = this;
		const BaseCollider2D* collider2 = &_other;

		for (size_t shapeID = 0; shapeID < 2; shapeID++) {
			if (shapeID == 1) {
				collider1 = &_other;
				collider2 = this;
			}

			for (size_t curVert = 0; curVert < collider1->tvert.size(); curVert++) {
				size_t nextVert = (curVert + 1) % collider1->tvert.size();	// 

				// Calculate the projecting axis 
				Vector2 axisProj = Vector2(
					-(collider1->tvert[nextVert].y - collider1->tvert[curVert].y),
					collider1->tvert[nextVert].x - collider1->tvert[curVert].x
				);

				float min_r1 = INFINITY, max_r1 = -INFINITY;
				for (size_t p = 0; p < collider1->tvert.size(); p++) {
					float q = (collider1->tvert[p].x * axisProj.x + collider1->tvert[p].y * axisProj.y);
					min_r1 = LMK_Min(min_r1, q);
					max_r1 = LMK_Max(max_r1, q);
				}

				float min_r2 = INFINITY, max_r2 = -INFINITY;
				for (size_t p = 0; p < collider2->tvert.size(); p++) {
					float q = (collider2->tvert[p].x * axisProj.x + collider2->tvert[p].y * axisProj.y);
					min_r2 = LMK_Min(min_r2, q);
					max_r2 = LMK_Max(max_r2, q);
				}

				if (!(max_r2 >= min_r1 && max_r1 >= min_r2)) return false;
			}
		}

		return true;
	}

#pragma warning (disable : 4244)
	//
	// 
	//
	_NODISCARD inline void Transform(Vector2 _offset, float _rotation, Vector2 _scale) {
		for (size_t i = 0; i < m_pS.vert.size(); i++) {
			tvert[i] = Vector2(
				(m_pS.vert[i].x * std::cosf(LMK_DtoR(_rotation))) * _scale.x - (m_pS.vert[i].y * std::sinf(LMK_DtoR(_rotation))) * _scale.y + _offset.x,
				(m_pS.vert[i].x * std::sinf(LMK_DtoR(_rotation))) * _scale.x + (m_pS.vert[i].y * std::cosf(LMK_DtoR(_rotation))) * _scale.y + _offset.y
			);
		}
	}
#pragma warning (default : 4244)

public: // Properties
	// The transformed verticies of the original PhysicsShape2D.
	std::vector<Vector2> tvert;

protected:
	// The original "model" of the Collider2D.
	PhysicsShape2D m_pS;

private:
	// Is this collider a trigger?
	bool m_isTrigger = false;

	// The local offset of this collider relative to the owning transform.
	Vector2 m_offset;
	// The size multiplier of this collider.
	Vector2 m_size;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | BOX COLLIDER 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

//
//
//
class BoxCollider2D : public BaseCollider2D {
public:
	inline BoxCollider2D(const Vector2& _size, float _edgeRadius) : BaseCollider2D(PhysicsShape2D{
		PhysicsShapeType2D::Polygon, _edgeRadius, {
			Vector2(-_size.x, _size.y),
			Vector2(_size.x, _size.y),
			Vector2(_size.x, -_size.y),
			Vector2(-_size.x, -_size.y),
		}
	}) {}
};

// +--------------------------------------------------------------------------------+
// |																				|
// | RAYCAST HIT 2D																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// 
//
struct RaycastHit2D {
	BaseCollider2D collider;

};

// +--------------------------------------------------------------------------------+
// |																				|
// | PHYSICS 2D																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
//
//
class Physics2D {
public: // Functions
	inline static RaycastHit2D Raycast(Vector2 origin, Vector2 direction, float distance = INFINITY, int layerMask = 0, float minDepth = -INFINITY, float maxDepth = INFINITY) {

	}

public: // Properties
	static float gravitationalAcceleration;
};
LMK_END

#endif // !LMK_PHYSICS_2D_H_