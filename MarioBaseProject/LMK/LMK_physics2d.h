#pragma once
#ifndef _LMK_PHYSICS_2D_H_
#define _LMK_PHYSICS_2D_H_

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
enum PhysicsShapeType2D {
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
	PhysicsShapeType2D shapeType = PhysicsShapeType2D::Polygon;

	// All PhysicsShapeType2D use this radius with the exception of the Polygon type.
	float radius = 0;

	// Coordinate of vertices relative to the origin (0, 0) of the shape.
	// Scales and rotation transformation are based around this original pivot point.
	std::vector<Vector2> vert;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | BASE COLLIDER 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Base class for collider types used with 2D physics.
//
class BaseCollider2D {
public:
	//
	// Creates a new BaseCollider2D.
	// 
	// @param _phyShape:
	//		The original "model" of the Collider.
	//
	inline BaseCollider2D(const PhysicsShape2D& _phyShape) 
		: m_pS(_phyShape), m_tvert(_phyShape.vert) {}

	//
	// Creates a new empty BaseCollider2D.
	// 
	// This Collider does not contains any vertex information.
	//
	inline BaseCollider2D() {}

public: // Functions
	//
	// 
	//
	_NODISCARD inline Vector2 ClosetPoint(Vector2 _position) {

	}

	//
	// Checking for overlapping colliders using SAT (Seperating Axis Theorem).
	//
	_NODISCARD inline bool OverlapWith(const BaseCollider2D& _other) {
		// For a detailed explaination of the Seperating Axis Theorem, see:
		// http://programmerart.weebly.com/separating-axis-theorem.html

		const BaseCollider2D* poly1 = this;
		const BaseCollider2D* poly2 = &_other;

		// We loop through each collider and check if any of its shadow projection
		// are overlapping with the other collider.
		for (size_t shapeID = 0; shapeID < 2; shapeID++) {
			if (shapeID == 1) {
				poly1 = &_other;
				poly2 = this;
			}

			for (size_t curVert = 0; curVert < poly1->m_tvert.size(); curVert++) {
				// Get the adjacent vertex id.
				// The reason for the modulo operator is to wrap the index back to the beginning 
				// when we are at the last vertex.
				size_t nextVert = (curVert + 1) % poly1->m_tvert.size();

				// Calculate the projecting axis.
				// The projecting axis is a vector perpendicular to the inspecting edge.
				Vector2 projAxis = Vector2::Perpendicular(poly1->m_tvert[nextVert] - poly1->m_tvert[curVert]);

				// Get the projected shadow length of the first collider.
				float min_r1 = INFINITY, max_r1 = -INFINITY;
				for (size_t p = 0; p < poly1->m_tvert.size(); p++) {
					// The projected point along the projecting axis
					float q = (poly1->m_tvert[p].x * projAxis.x + poly1->m_tvert[p].y * projAxis.y);

					min_r1 = LMK_Min(min_r1, q);
					max_r1 = LMK_Max(max_r1, q);
				}

				min_r1 -= m_pS.radius;
				max_r1 += m_pS.radius;

				// Get the projected shadow length of the second collider.
				float min_r2 = INFINITY, max_r2 = -INFINITY;
				for (size_t p = 0; p < poly2->m_tvert.size(); p++) {
					// The projected point along the projecting axis
					float q = (poly2->m_tvert[p].x * projAxis.x + poly2->m_tvert[p].y * projAxis.y);

					min_r2 = LMK_Min(min_r2, q);
					max_r2 = LMK_Max(max_r2, q);
				}

				min_r2 -= m_pS.radius;
				max_r2 += m_pS.radius;

				// If the projected shadows are not overlapping -> the polygons are not overlapping.
				if (!(max_r2 >= min_r1 && max_r1 >= min_r2)) return false;
			}
		}

		// If we get to this point -> all shadow projections overlapped -> polygon overlap.
		return true;
	}

#pragma warning (disable : 4244)
	//
	// Tranform to world space position from the original vertices.
	//
	_NODISCARD inline void Transform(Vector2 _offset, float _rotation, Vector2 _scale) {
		for (size_t i = 0; i < m_pS.vert.size(); i++) {
			m_tvert[i] = Vector2(
				(m_pS.vert[i].x * std::cosf(LMK_DtoR(_rotation))) * _scale.x - (m_pS.vert[i].y * std::sinf(LMK_DtoR(_rotation))) * _scale.y + _offset.x,
				(m_pS.vert[i].x * std::sinf(LMK_DtoR(_rotation))) * _scale.x + (m_pS.vert[i].y * std::cosf(LMK_DtoR(_rotation))) * _scale.y + _offset.y
			);
		}
	}
#pragma warning (default : 4244)
private:
	

public: // Accessors
	// Get the transformed vertices of this Collider.
	_NODISCARD inline std::vector<Vector2> vertices() {
		return m_tvert;
	}

	// Get the transformed vertex of given index.
	_NODISCARD inline Vector2 vertex(int _index) {
		return m_tvert[_index];
	}

private: // Properties
	PhysicsShape2D m_pS;	// The original "model" of the Collider2D.

	bool	m_isTrigger		= false;	// Is this collider a trigger?
	float	m_bounciness	= 0.0f;		// Get the bounciness used by the collider.
	float	m_friction		= 0.0f;		// Get the friction used by the collider.
	float	m_density		= 7.874f;	// The density of the collider used to calculate its mass (when auto mass is enabled).

	Vector2	m_offset	= Vector2::zero();	// The local offset of this collider relative to the owning transform.
	Vector2	m_scale		= Vector2::one();	// The size multiplier of this collider.

	std::vector<Vector2> m_tvert;	// The transformed verticies of the original PhysicsShape2D.
};

// +--------------------------------------------------------------------------------+
// |																				|
// | CIRCLE COLLIDER 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Collider for 2D physics representing an circle.
//
class CircleCollider2D : public BaseCollider2D {
public: // Constructors & Destructors
	// 
	// Creates a new CircleCollider2D.
	// 
	// @param _radius:
	//		Radius of the circle.
	//
	inline CircleCollider2D(float _radius) : BaseCollider2D(PhysicsShape2D{
		PhysicsShapeType2D::Circle, 
		_radius,
		{
			Vector2::zero()
		}
		}) {}

	//
	// Creates a new empty CircleCollider2D.
	//
	inline CircleCollider2D() : BaseCollider2D() {}
};

// +--------------------------------------------------------------------------------+
// |																				|
// | CAPSULE COLLIDER 2D															|
// |																				|
// +--------------------------------------------------------------------------------+

//
// A capsule-shaped primitive collider.
//
// Capsules are boxes with a semi - circle at each end.
//
class CapsuleCollider2D : public BaseCollider2D {
public: // Constructors & Destructors
	//
	// Create a new CapsuleCollider2D.
	// 
	// @param _hori:
	//		Is this capsule extending horizontally?
	//		If not, the capsule extends vertically by default.
	// @param _size:
	//		The width and height of the capsule area.
	//		The capsule will alter its geometry to fit into this area as best it can. 
	//		A capsule will automatically adjust the radius of the capsule ends as well as the capsule sides 
	//		to fit this area. When the capsule area is a 1:1 ratio, the capsule ends will fit together exactly 
	//		resulting in a circle only.
	//
	inline CapsuleCollider2D(bool _hori, const Vector2& _size) : BaseCollider2D(PhysicsShape2D{
		PhysicsShapeType2D::Capsule,
		_hori ? _size.y : _size.x,
		{
			Vector2(_hori ? LMK_Max(_size.x - _size.y,  0) : 0, _hori ? 0 : LMK_Max(_size.y - _size.x,  0)) * 0.5f * -1,
			Vector2(_hori ? LMK_Max(_size.x - _size.y,  0) : 0, _hori ? 0 : LMK_Max(_size.y - _size.x,  0)) * 0.5f
		}
		}) {}

	//
	// Creates a new empty CapsuleCollider2D.
	//
	inline CapsuleCollider2D() : BaseCollider2D() {}
};

// +--------------------------------------------------------------------------------+
// |																				|
// | BOX COLLIDER 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Collider for 2D physics representing an axis-aligned rectangle.
//
class BoxCollider2D : public BaseCollider2D {
public: // Constructors & Destructors
	//
	// Creates a new BoxCollider2D.
	// 
	// @param _size:
	//			The width and height of the rectangle.
	//			The X coordinate of the vector represents the width while the Y represents the height. 
	//			These values are specified relative to a center point, so the distance from the center 
	//			to the left edge is actually width/2.
	// @param _edgeRadius:
	//			Controls the radius of all edges created by the collider.
	//			The edge radius controls a radius extending around all edges of the box. 
	//			When an edge has zero radius it is effectively infinitely thin. 
	//			When an edge has a radius greater than zero, each edge acts like a 'capsule' shape with rounded ends. 
	//			This results in a box with rounded corners.
	//			Note: using the _edgeRadius does not effect the Rigidbody.mass even though the collision area has changed.
	//
	inline BoxCollider2D(const Vector2& _size, float _edgeRadius) : BaseCollider2D(PhysicsShape2D{
		PhysicsShapeType2D::Polygon, 
		_edgeRadius, 
		{
			Vector2(-_size.x, _size.y),
			Vector2(_size.x, _size.y),
			Vector2(_size.x, -_size.y),
			Vector2(-_size.x, -_size.y)
		}
		}) {}

	//
	// Creates a new empty BoxCollider2D.
	//
	inline BoxCollider2D() : BaseCollider2D() {}
};

// +--------------------------------------------------------------------------------+
// |																				|
// | POLYGON COLLIDER 2D															|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Collider for 2D physics representing an arbitrary polygon defined by its vertices.
//
class PolygonCollider2D : public BaseCollider2D {
public: // Constructors & Destructors
	//
	// Creates a new PolygonCollider2D.
	// 
	// @param _vertices:
	//		Corner points that define the collider's shape in local space.
	//
	inline PolygonCollider2D(const std::vector<Vector2>& _vertices) : BaseCollider2D(PhysicsShape2D{
		PhysicsShapeType2D::Polygon, 
		0, 
		_vertices
		}) {}

	//
	// Creates a new empty PolygonCollider2D.
	//
	inline PolygonCollider2D() : BaseCollider2D() {}
};

// +--------------------------------------------------------------------------------+
// |																				|
// | RIGIDBODY 2D																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Use these flags to constrain motion of the Rigidbody2D.
//
enum class RigidbodyConstraints2D {
	None			= 0x0000,								// No constraints.
	FreezePositionX	= 0x0001,								// Freeze motion along the X - axis.
	FreezePositionY	= 0x0002,								// Freeze motion along the Y - axis.
	FreezeRotation	= 0x0004,								// Freeze rotation along the Z - axis.
	FreezePosition	= FreezePositionX | FreezePositionY,	// Freeze motion along the X - axis and Y - axis.
	FreezeAll		= FreezeRotation | FreezePosition,		// Freeze rotation and motion along all axes.
};

//
// The physical behaviour type of the Rigidbody2D.
// 
// Controls the physical behaviour of a Rigidbody2D in how it should move, 
// react to forces and interact with the rest of the physics simulation.
//
enum class RigidbodyType2D {
	// Sets the Rigidbody2D to have dynamic behaviour.
	//
	// Dynamic behaviour causes the Rigidbody2D to react to gravity and applied forces including contacts 
	// with other Dynamic or Kinematic Rigidbody2D.
	// 
	// This type of Rigidbody2D should be moved using forces and never repositioned explicitly.
	// 
	// A dynamic Rigidbody2D will collide with all other Rigidbody2D body types.
	// 
	// When an attached Collider2D is set to trigger, it will always produce a trigger for 
	// any Collider2D attached to all other Rigidbody2D body types.
	Dynamic,

	// Sets the Rigidbody2D to have kinematic behaviour.
	//
	// Kinematic behaviour stops the Rigidbody2D from reacting to gravity or applied forces 
	// including contacts with other Kinematic or Static Rigidbody2D.
	// 
	// This type of Rigidbody2D can be moved by setting its Rigidbody2D.velocity or 
	// Rigidbody2D.angularVelocity or by being repositioned explicitly.
	// 
	// A Kinematic Rigidbody2D will only collide with a Dynamic Rigidbody2D body type. 
	// The exception to this is if Rigidbody2D.useFullKinematicContacts is set to true in which case 
	// it will collide with all other Rigidbody2D body types.
	// 
	// When an attached Collider2D is set to trigger, it will always produce a trigger for 
	// any Collider2D attached to all other Rigidbody2D body types.
	Kinematic,

	// Sets the Rigidbody2D to have static behaviour.
	//
	// Static behaviour stops the Rigidbody2D from reacting to gravity or applied forces 
	// including contacts with any other Rigidbody2D.
	// 
	// This type of Rigidbody2D should never repositioned explicitly. It is designed to never move.
	// 
	// A Static Rigidbody2D will only collide with a Dynamic Rigidbody2D body type. 
	// The exception to this is if Rigidbody2D.useFullKinematicContacts is set to true in which case 
	// it will also collide with any Kinematic body types.
	//
	// When an attached Collider2D is set to trigger, it will always produce a trigger for 
	// any Collider2D attached to Dynamic or Kinematic body types.
	Static,
};

//
// 
//
class Rigidbody2D {
private: // Properties
	float m_drag;			// 
	float m_angularDrag;	// 
	float m_inertia;		// 
	float m_mass;			// 
	float m_gravityScale;	// 

	Vector2	m_position;			//
	float	m_rotation;			//
	Vector2	m_velocity;			//
	float	m_angularVelocity;	//
	Vector2	m_centerOfMass;		//

	RigidbodyConstraints2D	m_constrains;				// 
	RigidbodyType2D			m_bodyType;					// 
	bool					m_useAutoMass;				// 
	bool					m_useFullKinematicContacts;	// 

	float totalForce;	// The total amount of force that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
	float totalTorque;	// The total amount of torque that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
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
// Global settings and helpers for 2D physics.
//
class Physics2D {
public: // Functions
	inline static RaycastHit2D Raycast(Vector2 origin, Vector2 direction, float distance = INFINITY, int layerMask = 0, float minDepth = -INFINITY, float maxDepth = INFINITY) {

	}

public: // Properties
	static float gravitationalAcceleration;
};
LMK_END

#endif // !_LMK_PHYSICS_2D_H_