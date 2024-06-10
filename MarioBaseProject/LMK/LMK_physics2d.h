#pragma once
#ifndef _LMK_PHYSICS_2D_H_
#define _LMK_PHYSICS_2D_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"
#include "LMK_systems.h"
#include "LMK_gameobj.h"
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
	// All polygon collision detection uses SAT (Separating Axis Theorem) convex polygons for performance reasons.
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
// | PHYSICS MATERIAL 2D															|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Asset type that defines the surface properties of a Collider2D.
//
struct PhysicsMaterial2D {
	float bounciness;	// The degree of elasticity during collisions.
	float friction;		// Coefficient of friction.
};

// +--------------------------------------------------------------------------------+
// |																				|
// | BASE COLLIDER 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

// Forward declaration of Rigidbody2D
class Rigidbody2D;

//
// Base class for collider types used with 2D physics.
//
class BaseCollider2D : public Component {
public: // Constructors & Destructors
	//
	// Creates a new BaseCollider2D.
	// 
	// @param _phyShape:
	//		The original "model" of the Collider.
	//
	inline BaseCollider2D(PhysicsShape2D* _phyShape) 
		: m_pS(_phyShape), m_tvert(_phyShape->vert), Component() {
		UpdateBounds();
	}

	//
	// Creates a new BaseCollider2D with no vertex.
	//
	inline BaseCollider2D() {}

public: // Functions
	//
	// Check if a collider overlaps a point in space.
	// 
	// @return
	//		True if _point overlap the collider. Otherwise false.
	//
	_NODISCARD inline bool OverlapPoint(Vector2 _point) {
		bool c = false; // True if point is inside the polygon else false.
		size_t n = m_tvert.size();
		for (size_t i = 0, j = n - 1; i < n; j = i++) {
			if (((m_tvert[i].y > _point.y) != (m_tvert[j].y > _point.y)) &&
				(_point.x < (m_tvert[j].x - m_tvert[i].x) * (_point.y - m_tvert[i].y) / (m_tvert[j].y - m_tvert[i].y) + m_tvert[i].x))
				c = !c;
		}
		return c;
	}

	//
	// Checking for overlapping colliders using SAT (Separating Axis Theorem).
	//
	_NODISCARD inline bool OverlapWith(const BaseCollider2D& _other) {
		// If the bounding box of this collider is not overlapping 
		// with the bounding box of the other collider.
		if (!m_bounds->Intersects(*_other.m_bounds)) return false;

		// For a detailed explanation of the Separating Axis Theorem, see:
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

				min_r1 -= m_pS->radius;
				max_r1 += m_pS->radius;

				// Get the projected shadow length of the second collider.
				float min_r2 = INFINITY, max_r2 = -INFINITY;
				for (size_t p = 0; p < poly2->m_tvert.size(); p++) {
					// The projected point along the projecting axis
					float q = (poly2->m_tvert[p].x * projAxis.x + poly2->m_tvert[p].y * projAxis.y);

					min_r2 = LMK_Min(min_r2, q);
					max_r2 = LMK_Max(max_r2, q);
				}

				min_r2 -= m_pS->radius;
				max_r2 += m_pS->radius;

				// If the projected shadows are not overlapping -> the polygons are not overlapping.
				if (!(max_r2 >= min_r1 && max_r1 >= min_r2)) return false;
			}
		}

		// If we get to this point -> all shadow projections overlapped -> polygon overlap.
		return true;
	}

#pragma warning (disable : 4244)
	//
	// Transform to world space position from the original vertices.
	//
	_NODISCARD inline void Transform(Vector2 _offset, float _rotation, Vector2 _scale) {
		for (size_t i = 0; i < m_pS->vert.size(); i++) {
			m_tvert[i] = Vector2(
				(m_pS->vert[i].x * std::cosf(LMK_DtoR(_rotation))) * _scale.x - (m_pS->vert[i].y * std::sinf(LMK_DtoR(_rotation))) * _scale.y + _offset.x,
				(m_pS->vert[i].x * std::sinf(LMK_DtoR(_rotation))) * _scale.x + (m_pS->vert[i].y * std::cosf(LMK_DtoR(_rotation))) * _scale.y + _offset.y
			);
		}
		UpdateBounds();
	}
#pragma warning (default : 4244)

private:
	//
	// Update the bounding box of this collider.
	//
	inline void UpdateBounds() {
		if (!m_bounds) m_bounds = std::make_unique<Bounds>();

		Vector2 min = m_tvert[0], max = m_tvert[0];
		for (auto vertex : m_tvert) {
			min = Vector2::Min(min, vertex);
			max = Vector2::Max(max, vertex);
		}

		m_bounds->setMinMax(min, max);
	}

public: // Accessors
	// Get the transformed vertices of this Collider.
	_NODISCARD inline std::vector<Vector2> vertices() {
		return m_tvert;
	}

	// Get the transformed vertex of given index.
	_NODISCARD inline Vector2 vertex(int _index) {
		return m_tvert[_index];
	}

	// Get the bounding box of this collider.
	_NODISCARD inline const Bounds bounds() {
		return *m_bounds;
	}

public: // Properties
	bool	isTrigger	= false;	// Is this collider a trigger?
	float	bounciness	= 0.0f;		// Get the bounciness used by the collider.
	float	friction	= 0.0f;		// Get the friction used by the collider.
	float	density		= 7.874f;	// The density of the collider used to calculate its mass (when auto mass is enabled).

	Vector2	offset	= Vector2::zero();	// The local offset of this collider relative to the owning transform.
	Vector2	scale	= Vector2::one();	// The size multiplier of this collider.

private:
	std::shared_ptr<Rigidbody2D> m_attachedRigidbody; // The Rigidbody2D attached to the Collider2D.

	std::unique_ptr<PhysicsShape2D>	m_pS		= nullptr;	// The original "model" of the Collider2D.
	std::unique_ptr<Bounds>			m_bounds	= nullptr;	// The world space bounding area of the collider.

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
	inline CircleCollider2D(float _radius) 
		: BaseCollider2D(new PhysicsShape2D {
		PhysicsShapeType2D::Circle, 
		_radius,
		{
			Vector2::zero()
		}
		}) {}

	//
	// Creates a new CircleCollider2D with radius of 1.
	//
	inline CircleCollider2D()
		: CircleCollider2D(1) {}
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
	inline CapsuleCollider2D(bool _hori, Vector2 _size) 
		: BaseCollider2D(new PhysicsShape2D {
		PhysicsShapeType2D::Capsule,
		_hori ? _size.y : _size.x,
		{
			Vector2(_hori ? LMK_Max(_size.x - _size.y,  0) : 0, _hori ? 0 : LMK_Max(_size.y - _size.x,  0)) * 0.5f * -1,
			Vector2(_hori ? LMK_Max(_size.x - _size.y,  0) : 0, _hori ? 0 : LMK_Max(_size.y - _size.x,  0)) * 0.5f
		}
		}) {}

	//
	// Creates a new CapsuleCollider2D with size of (0.5, 1) and extend vertically.
	//
	inline CapsuleCollider2D()
		: CapsuleCollider2D(false, Vector2(0.5f, 1)) {}
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
	//			NOTE: using the _edgeRadius does not effect the Rigidbody.mass even though the collision area has changed.
	//
	inline BoxCollider2D(Vector2 _size, float _edgeRadius) 
		: BaseCollider2D(new PhysicsShape2D {
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
	// Creates a new BoxCollider2D with size of (1, 1) and edge radius of 0.
	//
	inline BoxCollider2D() 
		: BoxCollider2D(Vector2::one(), 0) {}
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
	inline PolygonCollider2D(const std::vector<Vector2>& _vertices) 
		: BaseCollider2D(new PhysicsShape2D {
		PhysicsShapeType2D::Polygon, 
		0, 
		_vertices
		}) {}

	//
	// Creates a new PolygonCollider2D with no vertex data.
	//
	inline PolygonCollider2D()
		: BaseCollider2D() {}
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
public: // Constructors & Destructors
#pragma region Singleton
	inline Physics2D(const Physics2D&) = delete;

private:
	inline Physics2D() {}

public:
#pragma endregion

public: // Static Functions

public: // Accessors
	//
	inline static float gravitationalAcceleration() {
		return Instance.m_gravitationalAcceleration;
	}

private: // Properties
	static Physics2D Instance;

	float m_gravitationalAcceleration = 9.8f;
};

Physics2D Physics2D::Instance;

// +--------------------------------------------------------------------------------+
// |																				|
// | RIGIDBODY 2D																	|
// |																				|
// +--------------------------------------------------------------------------------+

ENUM_ENCAP_BEGIN(rgbody)
//
// Use these flags to constrain motion of the Rigidbody2D.
//
enum RigidbodyConstraints2D {
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
enum RigidbodyType2D {
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
ENUM_ENCAP_END

USE_ENUM_NS(rgbody);

//
// Rigidbody physics component for 2D sprites.
// 
// Adding a Rigidbody2D component to a sprite puts it under the control of the physics engine.
//
class Rigidbody2D : public Component {
public: // Typedef


public: // Constructors & Destructors
#pragma warning (disable : 26495)
	inline Rigidbody2D(
		RigidbodyType2D _bodyType,
		float m_drag,
		float m_angularDrag,
		float m_inertia,
		float m_mass,
		float m_gravityScale
	) {

	}

	inline Rigidbody2D() = default;
#pragma warning (default : 26495)

public:
	//
	// 
	//
	inline void Update() {
		velocity += Vector2::down() * Physics2D::gravitationalAcceleration() * m_gravityScale;
		transform->setPosition(transform->position() + velocity * Time::fixedDeltaTime());
	}

public: // Properties
	Vector2	velocity;	// 

private:
	RigidbodyType2D			m_bodyType;		//  

	float m_drag;			//  
	float m_angularDrag;	//  
	float m_inertia;		//  
	float m_mass;			//  
	float m_gravityScale;	//  

	bool m_useAutoMass;					//  
	bool m_useFullKinematicContacts;	// 

	float totalForce;	// The total amount of force that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
	float totalTorque;	// The total amount of torque that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
};

// +--------------------------------------------------------------------------------+
// |																				|
// | RIGIDBODY 2D																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
//
//
class PhysicsSystem {
public:
	inline bool Submit() {

	}

private:
	std::vector<std::shared_ptr<Rigidbody2D>> m_bodies;
};
LMK_END

#endif // !_LMK_PHYSICS_2D_H_