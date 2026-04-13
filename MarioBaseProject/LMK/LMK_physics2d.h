#pragma once
#ifndef _LMK_PHYSICS_2D_H_
#define _LMK_PHYSICS_2D_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"
#include "LMK_systems.h"
#include "LMK_gameobj.h"

LMK_BEGIN
IMPL_BEGIN
// +--------------------------------------------------------------------------------+
// |																				|
// | SIMPLEX SHAPE																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// A wrapper structure to represent simplex shapes.
// 
// In 1D, the simplex shape represents a line segment.
// In 2D, the simplex shape represents a triangle.
// 
class Simplex {
private: // Typedef
	// An array of Vector2.
	using storage_type = std::array<Vector2, 3>;
	// Iterator for storage_type.
	using iterator = storage_type::iterator;
	// The return type of storage_type.
	using value_type = storage_type::value_type;
	// An unsigned integral type.
	using size_type = storage_type::size_type;

public: // Constructors & Destructors
	inline Simplex()
		: m_verts({}), m_size(0) {}

public: // Operators Overloads
	inline operator std::vector<Vector2>() const {
		return { m_verts.begin(), m_verts.end() };
	}

	inline Simplex& operator=(std::initializer_list<Vector2> _verts) {
		LMK_CORE_ASSERT(_verts.size() < 4 && _verts.size() > 0,
			"lmk::impl::Simplex: operator=() - Invalid _verts size. (size in range [1 .. 3] expected)");
		for (auto iter = _verts.begin(); iter != _verts.end(); iter++) {
			m_verts[std::distance(_verts.begin(), iter)] = *iter;
		}
		m_size = _verts.size();
		return *this;
	}

	inline value_type& operator[](size_type _index) {
		LMK_CORE_ASSERT(_index < 3,
			"lmk::impl::Simplex: operator[] - Index out of range. (index in range [0 .. 2) expected)");
		return m_verts[_index];
	}

	inline value_type operator[](size_type _index) const {
		LMK_CORE_ASSERT(_index < 3,
			"lmk::impl::Simplex: operator[] - Index out of range. (index in range [0 .. 2) expected)");
		return m_verts[_index];
	}

public: // Functions
	inline void push_front(Vector2 _point) {
		m_verts = { _point, m_verts[0], m_verts[1] };
		m_size = LMK_Min(m_size + 1, 3);
	}

	[[nodiscard]]
    inline iterator begin() noexcept {
		return m_verts.begin();
	}

	[[nodiscard]]
    inline iterator end() noexcept {
		return m_verts.end() - (3 - m_size);
	}

	[[nodiscard]]
    inline size_type size() const noexcept {
		return m_size;
	}

private: // Properties
	storage_type m_verts;
	size_type m_size;
};
IMPL_END

// +--------------------------------------------------------------------------------+
// |																				|
// | 2D PHYSICS SHAPE 																|
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
	// All polygon collision detection uses convex polygons for performance reasons.
	Polygon,
};

//
// Represents an efficient low-level physics shape used by the 2D physics engine.
//
struct PhysicsShape2D {
	// The shape type determines how the vertices and radius are used by this PhysicsShape2D.
	PhysicsShapeType2D shapeType = PhysicsShapeType2D::Polygon;

	// The total number of vertices used to represent the shapeType.
	// See PhysicsShapeType2D for details on how each type uses different quantities of vertices to represent itself.
	size_t vertexCount;

	// The start index for the geometry of this shape within the PhysicsShapeGroup2D.
	// Multiple PhysicsShape2D in a PhysicsShapeGroup2D are represented as a single list of vertices. 
	// This index is the start of this shape within that list.
	size_t vertexStartIndex;

	// All PhysicsShapeType2D use this radius with the exception of the Polygon type.
	float radius = 0;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | 2D PHYSICS SHAPE GROUP 														|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Represents a group of PhysicsShape2D and their geometry.
// 
// A shape group represents multiple PhysicsShape2D of the same or mixed PhysicsShapeType2D along with their geometry.
// 
// It is comprised of a single list of vertices (GetShapeVertices) along with a list of PhysicsShape2D which refer to 
// specific ranges of those vertices (i.e. they index into the list of vertices). 
// 
// Some shape types (PhysicsShapeType2D) use a fixed number of vertices and some use a variable number of vertices 
// therefore this single vertices list is a compact and efficient representation for multiple PhysicsShape2D in a group.
// 
// The shape group can be created by using the following methods:
//	- Calling Collider2D.GetShapes where it would then represent all the shapes produced by that Collider2D
//	- Calling Rigidbody2D.GetShapes where it would then represent all the shapes produced by all the Collider2D attached to that Rigidbody2D.
//	- Manually populating with custom shapes by calling AddCircle, AddCapsule, AddPolygon, or AddBox.
//
class PhysicsShapeGroup2D {
private: // Typedef
	using vertices_storage	= std::vector<Vector2>;
	using shapes_storage	= std::vector<PhysicsShape2D>;

protected:
	friend class Collider2D;

public: // Constructors & Destructors
	//
	// Create a new empty PhysicsShapeGroup2D.
	//
	inline PhysicsShapeGroup2D() = default;

public: // Functions
	//
	// Add a copy of all the PhysicsShape2D and their geometry from the specified 
	// _physicsShapeGroup into this shape group. 
	// 
	// The specified _physicsShapeGroup is not modified.
	// 
	// @param _physicsShapeGroup:
	//		The PhysicsShapeGroup2D to add to this shape group.
	//
	inline void Add(const PhysicsShapeGroup2D& _physicsShapeGroup) noexcept {
		vertices_storage vertices;
		shapes_storage shapeList;

		_physicsShapeGroup.GetShapeData(vertices, shapeList);
		for (auto shape : shapeList) {
			// Insert vertices from _physicsShapeGroup to m_vertices.
			auto vertexStartIter = vertices.begin() + shape.vertexStartIndex;
			m_vertices.insert(m_vertices.end(), vertexStartIter, vertexStartIter + shape.vertexCount);

			// Modify shape.vertexStartIndex to the corresponding index in the vertex list of the current group.
			shape.vertexStartIndex = m_vertices.size();	

			// Push physics shape to current shape list and look up table.
			m_shapes.push_back(shape);
			m_startVertIndexMap.emplace(shape.vertexStartIndex, m_shapes.back());
		}
	}

	//
	// Add a circle shape (PhysicsShapeType2D.Circle) to the shape group.
	// 
	// A circle shape is comprised of a single vertex (center ) and a radius that extends around that vertex. 
	// This is the most memory efficient and fastest primitive to use in all use-cases.
	// 
	// @param _center:
	//		The center point of the circle shape. 
	//		This is analogous to Collider2D.offset.
	// @param _radius:
	//		The radius of the circle defining a radius around the center. 
	//		This is identical to CircleCollider2D.radius.
	// 
	// @return
	//		An integer represents the shape index the shape was added to in the PhysicsShapeGroup2D. 
	//		This index is used as the main reference when retrieving a shape.
	//
	inline int AddCircle(Vector2 _center, float _radius) {
		m_shapes.push_back(PhysicsShape2D {
			PhysicsShapeType2D::Circle,	// Shape type
			1,							// Vertex count
			m_vertices.size(),			// Vertex start index
			_radius						// Radius
			});

		m_startVertIndexMap.emplace(m_vertices.size(), m_shapes.back());

		m_vertices.push_back(_center);

		return m_shapes.size();
	}

	//
	// Add a capsule shape (PhysicsShapeType2D.Capsule) to the shape group.
	// 
	// A capsule shape is comprised of a single edge shape defined by two vertices (vertex0 and vertex1) 
	// and a radius that extends around the length of the edge and around the vertices.
	// 
	// @param _vertex0:
	//		The position of one end of a capsule shape. 
	//		This point represents the center point of a logical circle at the end of a capsule.
	// @param _vertex1:
	//		The position of the opposite end of a capsule shape. 
	//		This point represents the center point of a logical circle at the opposite end of a capsule.
	// @param _radius:
	//		The radius of the capsule defining a radius around the vertex0 and vertex1 and the area between them.
	// 
	// @return
	//		Returns the shape index the shape was added to in the PhysicsShapeGroup2D. 
	//		This index is used as the main reference when retrieving a shape.
	//
	inline int AddCapsule(Vector2 _vertex0, Vector2 _vertex1, float _radius) {
		m_shapes.push_back(PhysicsShape2D{
			PhysicsShapeType2D::Capsule,	// Shape type
			2,								// Vertex count
			m_vertices.size(),				// Vertex start index
			_radius							// Radius
			});

		m_startVertIndexMap.emplace(m_vertices.size(), m_shapes.back());

		m_vertices.insert(m_vertices.end(), { _vertex0, _vertex1 });

		return m_shapes.size();
	}

	//
	// Add a box shape (PhysicsShapeType2D.Polygon) to the shape group.
	// 
	// A box shape is a form of convex polygon with four vertices and an _edgeRadius that extends around the four edges.
	// The ability to add a box shape is a convenience only and does not represent an actual primitive shape.
	// 
	// @param _center:
	//		The center point of the box shape.
	//		This is analogous to Collider2D.offset.
	// @param _size:
	//		The size of the box.
	//		This is identical to BoxCollider2D.size.
	// @param _angle:
	//		The angle in degrees the box should be rotated around the center.
	// @param _edgeRadius:
	//		The radius extending around the edges of the box.
	//		This is identical to BoxCollider2D.edgeRadius.
	// 
	// @return
	//		An integer represents the shape index the shape was added to in the PhysicsShapeGroup2D. 
	//		This index is used as the main reference when retrieving a shape.
	//
	inline int AddBox(Vector2 _center, Vector2 _size, float _angle = 0.0f, float _edgeRadius = 0.0f) {
		m_shapes.push_back(PhysicsShape2D{
			PhysicsShapeType2D::Polygon,	// Shape type
			4,								// Vertex count
			m_vertices.size(),				// Vertex start index
			_edgeRadius						// Radius
			});

		m_startVertIndexMap.emplace(m_vertices.size(), m_shapes.back());

		Vector2 halfSize = _size / 2;
		Matrix3x3 rotateMatrix = Matrix3x3::Rotate(_angle);
		std::array<Vector2, 4> vertices = {
			Vector2{ _center.x - halfSize.x, _center.y + halfSize.y } * rotateMatrix,	// Top Left
			Vector2{ _center.x + halfSize.x, _center.y + halfSize.y } * rotateMatrix,	// Top Right
			Vector2{ _center.x + halfSize.x, _center.y - halfSize.y } * rotateMatrix,	// Bottom Right
			Vector2{ _center.x - halfSize.x, _center.y - halfSize.y } * rotateMatrix,	// Bottom Left
		};

		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

		return m_shapes.size();
	}

	//
	// Add a polygon shape (PhysicsShapeType2D.Polygon) to the shape group.
	// 
	// A polygon shape is comprised of multiple edges (line segments) defined by all the specified vertices.
	// 
	// @param _vertices:
	//		A list of vertices that represent a continuous set of edges of a convex polygon with each vertex 
	//		connecting to the following vertex to form each edge. 
	//		The final vertex implicitly connects to the first vertex. 
	//		The maximum allowed list length is defined by Physics2D.MaxPolygonShapeVertices.
	// 
	// @return
	//		An integer represents the shape index the shape was added to in the PhysicsShapeGroup2D. 
	//		This index is used as the main reference when retrieving a shape.
	//
	inline int AddPolygon(std::vector<Vector2> _vertices) {
		m_shapes.push_back(PhysicsShape2D{
			PhysicsShapeType2D::Polygon,	// Shape type
			_vertices.size(),				// Vertex count
			m_vertices.size(),				// Vertex start index
			0								// Radius
			});

		m_startVertIndexMap.emplace(m_vertices.size(), m_shapes.back());

		m_vertices.insert(m_vertices.end(), _vertices.begin(), _vertices.end());

		return m_shapes.size();
	}

	//
	// When destroying a shape at the specified shapeIndex, all other shapes that exist above the specified shapeIndex 
	// will have their shape indices updated appropriately.
	// 
	// @param _shapeIndex:
	//		The index of the shape stored in the PhysicsShapeGroup2D.
	//
	inline void DeleteShape(size_t _shapeIndex) {
		LMK_CORE_ASSERT(_shapeIndex >= m_shapes.size(),
			"lmk::PhysicsShapeGroup2D: GetShapeVertex() - _shapeIndex: Index out of range.");

		auto temp = m_shapes[_shapeIndex];
		m_shapes.erase(m_shapes.begin() + _shapeIndex);

		// Update shapes' vertexStartIndex.
		// All shapes that have 
		auto shapeIter = m_startVertIndexMap.lower_bound(temp.vertexStartIndex);

		for (auto iter = shapeIter; iter != m_startVertIndexMap.end(); iter++) {
			iter->second.vertexStartIndex -= temp.vertexCount;
		}

		m_startVertIndexMap.erase(shapeIter);
	}

	//
	// Clear all the vertices and shapes from the PhysicsShapeGroup.
	// 
	// This call clears the lists used to store both vertices and shapes but retains the current shape group capacity.
	//
	inline void Clear() {
		m_vertices.clear();
		m_shapes.clear();
	}

public: // Accessors
	//
	// Get the PhysicsShape2D stored at the specified shapeIndex.
	// 
	// NOTE: 
	// Since the PhysicsShape2D is a struct, the returned value is a copy of the shape. 
	// Therefore changing the shape afterwards will not change the PhysicsShapeGroup2D.
	// 
	// @param _shapeIndex:
	//		The index of the shape stored the PhysicsShapeGroup2D. 
	//		The shape index is zero-based with the shape group having a quantity of shapes specified by shapeCount.
	// 
	// @return
	//		Returns the PhysicsShape2D stored at the specified _shapeIndex.
	//
	[[nodiscard]]
    inline PhysicsShape2D GetShape(size_t _shapeIndex) const {
		LMK_CORE_ASSERT(_shapeIndex >= m_shapes.size(),
			"lmk::PhysicsShapeGroup2D: GetShapeVertex() - _shapeIndex: Index out of range.");

		return m_shapes[_shapeIndex];
	}

	//
	// Get a single vertex of a shape. 
	// 
	// The vertex index is zero-based with the shape having a quantity of vertex specified by PhysicsShape2D.vertexCount.
	// 
	// NOTE: 
	// When accessing multiple vertices, it is more efficient to do one of the following:
	//	- Get all the shape vertices using GetShapeVertices.
	//  - Get all the shape group vertices using GetShapeData.
	//  - Create the PhysicsShapeGroup2D with explicit shapes and vertices lists using the shape group constructor.
	// 
	// NOTE: 
	// Changing the returned value will not change the 
	//	
	// @param _shapeIndex:
	//		The index of the shape stored in the PhysicsShapeGroup2D. 
	//		The shape index is zero-based with the shape group having a quantity of shapes specified by shapeCount.
	// @param _vertexIndex:
	//		The index of the shape vertex stored in the PhysicsShapeGroup2D. 
	//		The vertex index is zero-based with the shape having a quantity of vertices specified by PhysicsShape2D.vertexCount.
	// 
	// @return 
	//		Returns the specified shape vertex.
	//
	[[nodiscard]]
    inline Vector2 GetShapeVertex(size_t _shapeIndex, size_t _vertexIndex) const {
		LMK_CORE_ASSERT(_shapeIndex >= m_shapes.size(), 
			"lmk::PhysicsShapeGroup2D: GetShapeVertex() - _shapeIndex: Index out of range.");
		LMK_CORE_ASSERT(_vertexIndex >= m_shapes[_shapeIndex].vertexCount, 
			"lmk::PhysicsShapeGroup2D: GetShapeVertex() - _vertexIndex: Index out of range.");

		return m_vertices[m_shapes[_shapeIndex].vertexStartIndex + _vertexIndex];
	}

	//
	// Get a copy of the shape vertices of the given  in the PhysicsShapeGroup2D.
	// 
	// NOTE: 
	// Since this is a copy, changing the specified shape vertices list afterwards 
	// will not change the PhysicsShapeGroup2D.
	// 
	// @param _shapeIndex:
	//		The index of the shape stored in the PhysicsShapeGroup2D. 
	//		The shape index is zero-based with the shape group having a quantity of shapes specified by shapeCount.
	// 
	// @return
	//		A list contains a copy of all the vertices of the given shape in the PhysicsShapeGroup2D.
	//
	[[nodiscard]]
    inline vertices_storage GetShapeVertices(size_t _shapeIndex) const {
		LMK_CORE_ASSERT(_shapeIndex >= m_shapes.size(),
			"lmk::PhysicsShapeGroup2D: GetShapeVertex() - _shapeIndex: Index out of range.");

		auto firstIter = m_vertices.begin() + m_shapes[_shapeIndex].vertexStartIndex;
		return { firstIter, firstIter + m_shapes[_shapeIndex].vertexCount };
	}

	//
	// Get a copy of both the shapes and vertices in the PhysicsShapeGroup2D.
	// 
	// NOTE: 
	// Since this is a copy, changing the specified shapes and vertices lists afterwards 
	// will not change the PhysicsShapeGroup2D.
	// 
	// @param _vertices:
	//		A list that will be populated with a copy of all the vertices in the PhysicsShapeGroup2D.
	// @param _shapes:
	//		A list that will be populated with a copy of all the shapes in the PhysicsShapeGroup2D.
	//
	inline void GetShapeData(vertices_storage& _vertices, shapes_storage& _shapes) const {
		_vertices = m_vertices;
		_shapes = m_shapes;
	}

public: // Mutators
	//
	// Set the radius of a shape.
	// 
	// @param _shapeIndex:
	//		The index of the shape stored in the PhysicsShapeGroup2D. 
	//		The shape index is zero-based with the shape group having a quantity of shapes specified by shapeCount.
	// @param _radius:
	//		The value to set the shape radius to.
	//
	inline void SetShapeRadius(size_t _shapeIndex, float _radius) {
		m_shapes[_shapeIndex].radius = _radius;
	}

	//
	// Set a single vertex of a shape.
	// 
	// @param _shapeIndex:
	//		The index of the shape stored in the PhysicsShapeGroup2D. 
	//		The shape index is zero-based with the shape group having a quantity of shapes specified by shapeCount.
	// @param _vertexIndex:
	//		The index of the shape vertex stored in the PhysicsShapeGroup2D. 
	//		The vertex index is zero-based with the shape having a quantity of vertex specified by PhysicsShape2D.vertexCount.
	// @param _vertex:
	//		The value to set the shape vertex to.
	//
	inline void SetShapeVertex(size_t _shapeIndex, size_t _vertexIndex, Vector2 _vertex) {
		LMK_CORE_ASSERT(_shapeIndex >= m_shapes.size(),
			"lmk::PhysicsShapeGroup2D: GetShapeVertex() - _shapeIndex: Index out of range.");
		LMK_CORE_ASSERT(_vertexIndex >= m_shapes[_shapeIndex].vertexCount,
			"lmk::PhysicsShapeGroup2D: GetShapeVertex() - _vertexIndex: Index out of range.");

		m_vertices[m_shapes[_shapeIndex].vertexStartIndex + _vertexIndex] = _vertex;
	}

public: // Accessors
	// Get the total number of vertices in the shape group used to represent all PhysicsShape2D within it.
	[[nodiscard]]
    inline size_t VertexCount() const noexcept {
		return m_vertices.size();
	}

	// Get the total number of PhysicsShape2D in the shape group.
	[[nodiscard]]
    inline size_t ShapeCount() const noexcept {
		return m_shapes.size();
	}

public: // Properties
	// Gets or sets a matrix that transforms the PhysicsShapeGroup2D vertices from local space into world space.
	// 
	// When retrieving a PhysicsShapeGroup2D via Rigidbody2D.GetShapes or Collider2D.GetShapes, 
	// this matrix will be set to the pose of the Rigidbody2D.
	// In case a Rigidbody2D is not available, the matrix it set to Matrix4x4.identity.
	Matrix3x3 localToWorldMatrix = Matrix3x3::identity;

private:
	// The list of vertices in the group.
	vertices_storage m_vertices;

	// The list of shapes in the group.
	shapes_storage m_shapes;

	// A lookup map to link a startVertexIndex property of a PhysicsShape2D to its data.
	// This is used for easy access during insertion or deletion of vertices in the PhysicsShapeGroup2D.
	std::map<size_t, shapes_storage::reference> m_startVertIndexMap;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | 2D PHYSICS MATERIAL															|
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
// | 2D COLLIDER CONTACT															|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Represents the separation or overlap of two Collider2D.
// 
// The ColliderDistance2D primarily defines a point on the exterior of each Collider2D along with the distance between those two points.
// 
// This can be used to resolving overlaps (collision) between two Collider2D.
//
struct ColliderDistance2D {
	// The distance between two colliders.
	//
	// The distance between two colliders is defined as the length of a Vector2 between pointA and pointB.
	// The distance between these points can be positive indicating that the Collider2D are separated (not overlapped), 
	// zero indicating that they are touching (but not overlapped) or negative indicating that they are overlapped.
	float distance;

	// A normalized vector that points from pointB to pointA.
	// 
	// Scaling this property with the distance will produce a vector that can be used to move the Collider2D so that 
	// they are no longer overlapped (if the distance is negative) or so they are touching (if the distance is positive).
	Vector2	normal;		

	// A point on a Collider2D that is a specific distance away from pointB.
	// 
	// This point will always be a vertex in the PhysicsShape2D of a Collider2D.
	Vector2 pointA;

	// A point on a Collider2D that is a specific distance away from pointA.
	//  
	// This point lies on the perimeter of a Collider2D.
	Vector2 pointB;

	// Gets whether the distance represents an overlap or not.
	bool isOverlapped = true;

	// Gets whether the distance is valid or not.
	//
	// When a function returns a ColliderDistance2D struct, it may not be valid because the Collider2D used as distance 
	// references were not valid in some way due to them being disabled or not having any valid shapes. 
	// 
	// This property indicates whether the other distance properties can be used and are valid or not.
	//
	// This is useful for physic simulation to knows which collision needs to be resolved.
	bool isValid = true;
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
class Collider2D : public Behaviour {
protected: // Typedef
	struct ProjectionResult {
		float minDis;	// The minimum distance on the projection axis.
		float maxDis;	// The maximum distance on the projection axis.
	};

	struct Polygon {
		inline explicit Polygon(float _radius, const std::vector<Vector2>& _vertices)
			: radius(_radius), vertices(_vertices) {}

		float radius;					// The edge radius value of the polygon.
		std::vector<Vector2> vertices;	// The vertices of the polygon.
	};

public: // Constructors & Destructors
	//
	// Copy constructor.
	//
	inline Collider2D(const Collider2D& _other) {
		m_shapes = new PhysicsShapeGroup2D{ *_other.m_shapes };
		m_bounds = new Bounds{ *_other.m_bounds };
	}

	//
	// Create a new Collider2D with no vertex data.
	//
	inline Collider2D() = default;

	inline ~Collider2D() {
		delete m_shapes;
		delete m_bounds;
		m_shapes = nullptr;
		m_bounds = nullptr;
	}

public: // Functions
	//
	// Calculates the minimum separation of this collider against another collider.
	// 
	// A valid collider must be provided for the ColliderDistance2D to be valid. 
	// 
	// If there are any problems with collider or this Collider2D such as they are disabled or 
	// do not contain any collision shapes then the separation will be invalid as indicated by ColliderDistance2D.isValid.
	//
	[[nodiscard]]
    inline ColliderDistance2D Distance(const Collider2D& _other) {
		ColliderDistance2D resultDistance;
		impl::Simplex resultSimplex;

		if (CollisionDetection_GJK(*this, _other, resultSimplex)) {
			resultDistance = EPA(*this, _other, resultSimplex, 30);
			resultDistance.isOverlapped = true;
		}
		else {
			resultDistance.isOverlapped = false;
		}

		resultDistance.isValid =
			this->IsActiveAndEnabled() &&
			_other.IsActiveAndEnabled() &&
			this->m_shapes->VertexCount() > 0 &&
			_other.m_shapes->VertexCount() > 0;

		return resultDistance;
	}

	//
	// Check if a collider overlaps a point in space.
	// 
	// @return
	//		True if _point overlap the collider, otherwise false.
	//
	[[nodiscard]]
    inline bool OverlapPoint(Vector2 _point) const {
		bool overlap = false;

		// We convert the point to local space since vertices of the polygon(s)
		// are stored in local space. 
		Vector2 localPoint = transform->InverseTransformPoint(_point);

		for (size_t shapeID = 0; shapeID < m_shapes->ShapeCount(); shapeID++) {
			auto vertices = m_shapes->GetShapeVertices(shapeID);
			size_t n = vertices.size();

			for (size_t i = 0; i < n; i++) {
				Vector2 pointA = vertices[i];
				Vector2 pointB = vertices[(i + 1) % n];

				// Check if point is inside edge radius of the current edge.
				if ((localPoint - ClosestPointOnLineSegment(localPoint, pointA, pointB)).GetMagnitude() 
					<= m_shapes->GetShape(shapeID).radius)
					return true;

				// Check if point is inside polygon disregarding edge radius.
				if (((pointA.y > localPoint.y) != (pointB.y > localPoint.y)) &&
					(localPoint.x < (pointB.x - pointA.x) * (localPoint.y - pointA.y) / (pointB.y - pointA.y) + pointA.x))
					overlap = !overlap;
			}

			if (overlap) return true;
		}

		return false;
	}

	//
	// Checking if for overlapping colliders using GJK (Gilbert-Johnson-Keerthi) algorithm.
	// 
	// @param _other:
	//		The other collider to check against.
	// 
	// @return
	//		True if _other overlap the collider, otherwise false.
	//
	[[nodiscard]]
    inline bool OverlapCollider(const Collider2D& _other) const {
		// If the bounding box of the colliders do not overlap -> abort.
		if (!m_bounds->Intersects(*_other.m_bounds)) return false;

		// SAT is slightly faster than GJK in most cases with non-complex 2D polygons,
		// and since this function only need to check for overlap, we prefer SAT over GJK.
		return CollisionDetection_SAT(*this, _other);
	}

	//
	// Find a point on or in this collider that is closest to the specified position.
	// 
	// @param _point:
	//		The position from which to find the closest point on this collider.
	// @param _perimeterOnly:
	//		If true, this will only checks for points on the perimeter of this collider.
	// 
	// @return 
	//		A point on or in this collider that is closest to the specified position.
	//
	[[nodiscard]]
    inline Vector2 ClosetPoint(Vector2 _point, bool _perimeterOnly = true) const {
		// If we are checking for points inside the collider.
		if (!_perimeterOnly) {
			// If the polygon contains the point -> it is the closest point to itself.
			if (OverlapPoint(_point)) return _point;
		}

		float minDistance = INFINITY;
		Vector2 closestPoint;

		for (size_t shapeID = 0; shapeID < m_shapes->ShapeCount(); shapeID++) {
			auto vertices = m_shapes->GetShapeVertices(shapeID);
			size_t n = vertices.size();

			// Find closest point on each edge.
			for (size_t i = 0; i < n; i++) {
				Vector2 closest = ClosestPointOnLineSegment(_point, vertices[i], vertices[(i + 1) % n]);
				float dist = Vector2::Distance(_point, closest);
				if (dist < minDistance) {
					minDistance = dist;
					closestPoint = closest;
				}
			}
		}

		return closestPoint;
	}

private:
	//
	// Update the world space bounding box of this collider.
	//
	inline void RecalculateBounds() noexcept {
		Vector2 min = Vector2::positiveInfinity;
		Vector2 max = Vector2::negativeInfinity;

		for (size_t shapeID = 0; shapeID < m_shapes->ShapeCount(); shapeID++) {
			auto shape = m_shapes->GetShape(shapeID);
			auto vertices = m_shapes->GetShapeVertices(shapeID);
			auto radiusVec = Vector2{ shape.radius, shape.radius };

			for (auto vertex : vertices) {
				min = Vector2::Min(min - radiusVec, vertex - radiusVec);
				max = Vector2::Max(max + radiusVec, vertex + radiusVec);
			}
		}

		m_bounds->SetMinMax(
			transform->TransformPoint(min), 
			transform->TransformPoint(max) 
		);
	}

private: // Static Functions
	//
	// Find the closest point to _point on a line segment defined by _lineA and _lineB.
	// 
	// @return 
	//		A vector represents a point on the line segment that is the closest to _point.
	//
	[[nodiscard]]
    inline static Vector2 ClosestPointOnLineSegment(Vector2 _point, Vector2 _lineA, Vector2 _lineB) noexcept {
		Vector2 ap = _point - _lineA;
		Vector2 line = _lineB - _lineA;

		// This represents the position of the closest point on the line segment relative to point _lineA.
		float t = Vector2::Dot(ap, line) / line.GetSqrMagnitude();

		if (t < 0.0f) {
			return _lineA;
		}
		else if (t > 1.0f) {
			return _lineB;
		}
		else {
			return _lineA + line * t;
		}
	}

	//
	// Project _point onto _projectAxis.
	// 
	// @param _point:
	//		The point to project.
	// @param _projectAxis:
	//		The vector represents as the projecting axis.
	// 
	// @return 
	//		A vector represents the projected point on the projecting axis.
	//
	[[nodiscard]]
    inline static Vector2 ProjectPoint(Vector2 _point, Vector2 _projectAxis) noexcept {
		float scalar = Vector2::Dot(_point, _projectAxis) / _projectAxis.GetSqrMagnitude();
		return _projectAxis * scalar;
	}

	//
	// Project a polygon onto a given axis (represented by a normal vector).
	// 
	// @param _collider:
	//		The collider to receive vertex information from.
	// @param _projectAxis:
	//		The vector represents as the projecting axis.
	// 
	// @return 
	//		A ProjectionResult structure represents the result of the projected polygon.
	//
	[[nodiscard]]
    inline static ProjectionResult ProjectPolygon(const Polygon& _polygon, Vector2 _projectAxis) noexcept {
		float minDis = INFINITY, maxDis = -INFINITY;

		for (auto vert : _polygon.vertices) {
			// The projected point along the projecting axis
			float dis = Vector2::Dot(vert, _projectAxis);

			minDis = LMK_Min(dis, minDis);
			maxDis = LMK_Max(dis, maxDis);
		}

		return { 
			minDis - _polygon.radius,
			maxDis + _polygon.radius,
		};
	}

	//
	// Find the furthest vertex in the polygon with the given direction.
	// 
	// @param _polygon:
	//		A list of Vector2 represents the vertices of the polygon.
	// @param _edgeRadius:
	//		
	// @param _direction:
	//		A vector represents the direction to check for.
	//  
	// @return 
	//		The local space coordinate represents the furthest vertex in the given direction.
	//
	[[nodiscard]]
	inline static Vector2 FindFurthestVertex(const Polygon& _polygon, Vector2 _direction) noexcept {
		Vector2 maxPoint;
		float maxDis = -INFINITY;

		// Find furthest projection of vertices on _direction.
		for (Vector2 vert : _polygon.vertices) {
			float distance = Vector2::Dot(vert, _direction);
			if (distance > maxDis) {
				maxDis = distance;
				maxPoint = vert;
			}
		}
		// Add physics shape radius to the result vertex.
		// This generalizes the function for all non-polygon shapes.
		maxPoint += _direction.GetNormalized() * _polygon.radius;

		return maxPoint;
	}

	//
	// The support function used for GJK algorithm.
	// 
	// This function returns the most extreme vertex of the Minkowski differences 
	// of the 2 polygons in the given direction.
	// 
	// NOTE:
	// The direction is reversed for the second polygon.
	// 
	// @param _polygonA:
	//		The first collider.
	// @param _polygonB:
	//		The second collider.
	// @param _direction:
	//		A vector represents the direction to check for.
	// 
	// @return 
	//		A vector represents the support point.
	//
	[[nodiscard]]
	inline static Vector2 Support(const Polygon& _polygonA, const Polygon& _polygonB, Vector2 _direction) noexcept {
		return FindFurthestVertex(_polygonA, _direction) - FindFurthestVertex(_polygonB, -_direction);
	}

	//
	// Implementation of SAT (Separating Axis Theorem) algorithm for overlap detection 
	// between two simple convex polygons.
	//
	[[nodiscard]]
	inline static bool SATCore(const Polygon& _polygonA, const Polygon& _polygonB) {
		// For a detailed explanation of the Separating Axis Theorem algorithm, see:
		// http://programmerart.weebly.com/separating-axis-theorem.html

		size_t n = _polygonA.vertices.size();

		// Loop through all edges of the first polygon.
		for (size_t curVert = n - 1, nextVert = 0; nextVert < n; curVert = nextVert++) {
			// Calculate the projecting axis.
			// The projecting axis is a vector perpendicular to the inspecting edge.
			Vector2 projectAxis = Vector2::Perpendicular(_polygonA.vertices[nextVert] - _polygonA.vertices[curVert]);

			// Get the projected shadow length of the first polygon.
			auto projectA = ProjectPolygon(_polygonA, projectAxis);
			projectA.minDis -= _polygonA.radius;
			projectA.maxDis += _polygonA.radius;

			// Get the projected shadow length of the second polygon.
			auto projectB = ProjectPolygon(_polygonB, projectAxis);
			projectB.minDis -= _polygonB.radius;
			projectB.maxDis += _polygonB.radius;

			// Calculate distance between projected shadow polygons distance.
			float currentDistance = LMK_Min(projectA.maxDis, projectB.maxDis) - LMK_Max(projectA.minDis, projectB.minDis);

			// Negative distance means the two polygons are not overlapping.
			if (currentDistance < 0) return false;
		}
	}

	//
	// Implementation of SAT (Separating Axis Theorem) algorithm for collision detection 
	// between colliders with multiple physical shapes.
	//
	[[nodiscard]]
    inline static bool CollisionDetection_SAT(const Collider2D& _colliderA, const Collider2D& _colliderB) {
		// Loop through all shapes of the first collider.
		for (size_t shapeIDA = 0; shapeIDA < _colliderA.m_shapes->ShapeCount(); shapeIDA++) {
			auto shapeA = _colliderA.m_shapes->GetShape(shapeIDA);
			auto verticesA = _colliderA.m_shapes->GetShapeVertices(shapeIDA);
			_colliderA.transform->TransformPoints(verticesA);

			// Loop through all shapes of the second collider.
			for (size_t shapeIDB = 0; shapeIDB < _colliderB.m_shapes->ShapeCount(); shapeIDB++) {
				auto shapeB = _colliderB.m_shapes->GetShape(shapeIDB);
				auto verticesB = _colliderB.m_shapes->GetShapeVertices(shapeIDB);
				_colliderB.transform->TransformPoints(verticesB);

				if (SATCore(Polygon{ shapeA.radius, verticesA }, Polygon{ shapeB.radius, verticesB })) {
					return true;
				}
			}
		}

		return false;
	}

	//
	// Implementation of GJK (Gilbert�Johnson�Keerthi) algorithm for overlap detection
	// between two polygons.
	// 
	// @param _resultSimplex:
	//		The result simplex if the colliders overlapped.
	//
	[[nodiscrad]]
	inline static bool GJKCore(const Polygon& _polygonA, const Polygon& _polygonB, impl::Simplex& _resultSimplex) {

	}

	//
	// Implementation of GJK (Gilbert�Johnson�Keerthi) algorithm for collision detection
	// between two colliders.
	// 
	// @param _resultSimplex:
	//		The result simplex if the colliders overlapped.
	//
	[[nodiscard]]
    inline static bool CollisionDetection_GJK(const Collider2D& _colliderA, const Collider2D& _colliderB, impl::Simplex& _resultSimplex) {
		// For a detailed explanation of the Gilbert�Johnson�Keerthi algorithm, see:
		// https://medium.com/@mbayburt/walkthrough-of-the-gjk-collision-detection-algorithm-80823ef5c774
		// and
		// https://winter.dev/articles/gjk-algorithm

		// Get initial support point in any direction (vector (1, 0) in this case)
		Vector2 support = Support(_colliderA, _colliderB, Vector2::right);

		impl::Simplex simplex;
		simplex.push_front(support);

		// Set the next direction towards the origin.
		Vector2 direction = -support;

		while (true) {
			support = Support(_colliderA, _colliderB, direction);

			// If the support is not pass the origin.
			if (Vector2::Dot(support, direction) <= 0) {
				return false; // No collision
			}

			simplex.push_front(support);

			if (GJK_HandleSimplex(simplex, direction)) {
				_resultSimplex = simplex;
				return true;
			}
		}
	}

	[[nodiscard]]
    inline static bool GJK_HandleSimplex(impl::Simplex& _simplex, Vector2& _direction) {
		if (_simplex.size() == 2)
			return GJK_LineCase(_simplex, _direction);
		else
			return GJK_TriangleCase(_simplex, _direction);
	}

	[[nodiscard]]
    inline static bool GJK_LineCase(impl::Simplex& _simplex, Vector2& _direction) {
		Vector2 a = _simplex[0];
		Vector2 b = _simplex[1];

		Vector2 ab = b - a;
		Vector2 ao = -a;

		if (Vector2::Dot(ab, ao) > 0) {
			// We use triple product to calculate the correct normal orientation.
			_direction = Vector3::Cross(Vector3::Cross(ab, ao), ab);
		}
		else {
			_simplex = { a };
			_direction = ao;
		}

		return false;
	}

	[[nodiscard]]
    inline static bool GJK_TriangleCase(impl::Simplex& _simplex, Vector2& _direction) {
		Vector2 a = _simplex[0];
		Vector2 b = _simplex[1];
		Vector2 c = _simplex[2];

		Vector2 ab = b - a;
		Vector2 ac = c - a;
		Vector2 ao = -a;
		
		Vector3 abc = Vector3::Cross(ab, ac);

		// Origin is in region outside AC.
		if (Vector2::Dot(Vector3::Cross(abc, ac), ao) > 0) {
			// If AC and AO are pointing in the same general direction.
			if (Vector2::Dot(ac, ao) > 0) {
				// Adjust simplex to AC.
				_simplex = { a, c };
				_direction = Vector3::Cross(Vector3::Cross(ac, ao), ac);
			}
			else {
				return GJK_LineCase(_simplex = { a, b }, _direction);
			}
		}
		else {
			// Origin is in region outside AB.
			if (Vector2::Dot(Vector3::Cross(ab, abc), ao) > 0) {
				return GJK_LineCase(_simplex = { a, b }, _direction);
			}
			// If both case was false, origin is inside triangle.
			else {
				return true;
			}
		}

		return false;
	}

	[[nodiscard]]
    inline static ColliderDistance2D EPA(const Collider2D& _colliderA, const Collider2D& _colliderB, const impl::Simplex& _simplex, int _maxIterations) noexcept {
		size_t minIndex = 0;
		float minDistance = INFINITY;
		Vector2 minNormal;
		std::vector<Vector2> polytope = _simplex;

		int count = 0;
		while (minDistance == INFINITY) {
			count++;

			// Find the closest edge from the origin.
			size_t n = polytope.size();
			for (size_t i = 0; i < n; i++) {
				size_t j = (i + 1) % n;

				Vector2 currVert = polytope[i];
				Vector2 nextVert = polytope[j];
				Vector2 normal = Vector2::Perpendicular(nextVert - currVert).GetNormalized();
				float distance = Vector2::Dot(currVert, normal);

				// If the distance is negative -> normal is facing the wrong direction -> flip normal direction.
				if (distance < 0) {
					distance *= -1;
					normal *= -1;
				}

				if (distance < minDistance) {
					minDistance = distance;
					minNormal = normal;
					minIndex = j;
				}
			}

			// Get the support point in the direction of the normal. (if have)
			Vector2 support = Support(_colliderA, _colliderB, minNormal);
			float sDistance = Vector2::Dot(support, minNormal);

			if (std::abs(sDistance - minDistance) > 0.001f && count < _maxIterations) {
				minDistance = INFINITY;
				polytope.insert(polytope.begin() + minIndex + 1, support);
			}
		}

		return { minDistance, -minNormal };
	}

public: // Accessors
	// Get the bounding box of this collider.
	[[nodiscard]]
    inline const Bounds GetBounds() {
		return *m_bounds;
	}

public: // Properties
	bool	isTrigger	= false;	// Is this collider a trigger?
	float	bounciness	= 0.0f;		// Get the bounciness used by the collider.
	float	friction	= 0.5f;		// Get the friction used by the collider.
	float	density		= 7.874f;	// The density of the collider used to calculate its mass (when auto mass is enabled).

	Vector2	offset	= Vector2::zero;	// The local offset of this collider relative to the owning transform.
	Vector2	scale	= Vector2::one;		// The Count multiplier of this collider.

protected:
	PhysicsShapeGroup2D* m_shapes = new PhysicsShapeGroup2D();	// The original "model" of the Collider2D.

	Bounds*	m_bounds = new Bounds();	// The world space bounding area of the collider.

private:
	Rigidbody2D* m_attachedRigidbody = nullptr;	// The Rigidbody2D attached to the Collider2D.
};

// +--------------------------------------------------------------------------------+
// |																				|
// | CIRCLE COLLIDER 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Collider for 2D physics representing an circle.
//
class CircleCollider2D final : public Collider2D {
public: // Constructors & Destructors
	// 
	// Create a new CircleCollider2D.
	// 
	// @param _radius:
	//		Radius of the circle.
	//
	inline CircleCollider2D(float _radius) 
		: Collider2D() {
		m_shapes->AddCircle(Vector2::zero, _radius);
	}

	//
	// Create a new CircleCollider2D with radius of 1.
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
// Capsules are boxes with a semi - circle Get each end.
//
class CapsuleCollider2D final : public Collider2D {
public: // Constructors & Destructors
	//
	// Create a new CapsuleCollider2D.
	// 
	// @param _horiz:
	//		Is this capsule extending horizontally?
	//		If not, the capsule extends vertically by default.
	// @param _size:
	//		The width and height of the capsule area.
	//		The capsule will alter its geometry to fit into this area as best it can. 
	//		A capsule will automatically adjust the radius of the capsule ends as well as the capsule sides 
	//		to fit this area. When the capsule area is a 1:1 ratio, the capsule ends will fit together exactly 
	//		resulting in a circle only.
	//
	inline CapsuleCollider2D(bool _horiz, Vector2 _size) 
		: Collider2D() {
		Vector2 vert;
		if (_horiz) 
			vert = Vector2{ LMK_Max(_size.x - _size.y, 0), 0 };
		else
			vert = Vector2{ 0, LMK_Max(_size.y - _size.x,  0) };

		m_shapes->AddCapsule(vert * -0.5f, vert * 0.5f, _horiz ? _size.y : _size.x);
	}

	//
	// Create a new CapsuleCollider2D with Count of (0.5, 1) and extend vertically.
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
class BoxCollider2D final : public Collider2D {
public: // Constructors & Destructors
	//
	// Create a new BoxCollider2D.
	// 
	// @param _size:
	//			The width and height of the rectangle.
	//			The X coordinate of the vector represents the width while the Y represents the height. 
	//			These values are specified relative to a center point, so the distance from the center 
	//			to the left edge is actually _size.x / 2.
	// @param _edgeRadius:
	//			Controls the radius of all edges created by the collider.
	//			The edge radius controls a radius extending around all edges of the box. 
	//			When an edge has zero radius it is effectively infinitely thin. 
	//			When an edge has a radius greater than zero, each edge acts like a 'capsule' shape with rounded ends. 
	//			This results in a box with rounded corners.
	//			NOTE: using the _edgeRadius does not effect the Rigidbody2D.mass even though the collision area has changed.
	//
	inline BoxCollider2D(Vector2 _size, float _edgeRadius) 
		: Collider2D() {
		m_shapes->AddBox(Vector2::zero, _size, 0, _edgeRadius);
	}

	//
	// Create a new BoxCollider2D with Count of (1, 1) and edge radius of 0.
	//
	inline BoxCollider2D() 
		: BoxCollider2D(Vector2::one, 0) {}
};

// +--------------------------------------------------------------------------------+
// |																				|
// | POLYGON COLLIDER 2D															|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Collider for 2D physics representing an arbitrary polygon defined by its vertices.
//
class PolygonCollider2D final : public Collider2D {
public: // Constructors & Destructors
	//
	// Create a new PolygonCollider2D.
	// 
	// @param _vertices:
	//		Corner points that define the collider's shape in local space.
	//
	inline PolygonCollider2D(const std::vector<Vector2>& _vertices) 
		: Collider2D() {
		m_shapes->AddPolygon(_vertices);
	}

	//
	// Create a new PolygonCollider2D with no vertex data.
	//
	inline PolygonCollider2D()
		: Collider2D() {}

public: // Functions

private:
	//
	// Check if the vertex at _vertexIndex is  formed by 2 vectors is reflective.
	// 
	// A vertex v of P is called convex if the internal angle of P at v is less than
	// 180o, and reflex otherwise.
	//
	[[nodiscard]]
    inline bool IsReflex(size_t _vertexIndex) {

	}

	//
	// Check if a triangle formed by 3 consecutive points of a polygon is defined as an "ear".
	// 
	// An "ear" of a polygon is defined as a triangle that 
	// 
	// @param _pointIndex:
	//		The index of the middle point of the triangle.
	//		The 2 other points of the triangle are sampled from
	//		the 2 indices before and after this index.
	//
	[[nodiscard]]
    inline bool IsEar(size_t _pointIndex) const {
		LMK_CORE_ASSERT(_pointIndex >= 0 && _pointIndex < m_vertices.size(),
			"lmk::PhysicsShape2D: IsEar() - Invalid _pointIndex parameter.");

		Vector2 A = m_vertices[(_pointIndex - 1)];
		Vector2 B = m_vertices[_pointIndex];
		Vector2 C = m_vertices[(_pointIndex - 1)];

		// If the angle at the point of _pointIndex is >= 180 degree -> not an ear.
		if (true) return false;

		return true;
	}

	//
	// Perform polygon partition using "fancy" ear clipping algorithm.
	// 
	// @return 
	//		An std::vector of PhysicsShape2D represents the result convex
	//		polygons after decomposition.
	//
	[[nodiscard]]
    inline std::vector<PhysicsShape2D> FancyEarClipping() noexcept {

	}

private: // Static Functions
	//
	// Check if a triangle contains a point.
	// 
	// @param _triangle:
	//		An array with 3 Vector2 represents the points of the triangle.
	// @param _point:
	//		A Vector2 represents the point to check.
	// 
	// @return
	//		Is the point contained within the triangle?
	// 
	[[nodiscard]]
    inline static bool TriangleContains(Vector2 _triangle[3], Vector2 _point) {
		float crossAB = Vector2::Cross(_triangle[0] - _point, _triangle[1] - _point);
		float crossBC = Vector2::Cross(_triangle[1] - _point, _triangle[2] - _point);
		float crossAC = Vector2::Cross(_triangle[2] - _point, _triangle[0] - _point);

		return (crossAB >= 0 && crossBC >= 0 && crossAC >= 0)
			|| (crossAB <= 0 && crossBC <= 0 && crossAC <= 0);
	}
};

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

USE_ENUM_NS(rgbody)

//
// Rigidbody physics component for 2D sprites.
// 
// Adding a Rigidbody2D component to a sprite puts it under the control of the physics engine.
//
class Rigidbody2D : public Component {
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
		m_attachedCollider = gameObject->GetComponent<Collider2D>();
	}

	inline Rigidbody2D() = default;
#pragma warning (default : 26495)

public: // Functions
	// 
	// Resolve all current collisions of this body.
	//
	inline void ResolveCollision(const Rigidbody2D& _other) {
		
	}

private:
	//
	// Move the body by _amount.
	//
	inline void Move(Vector2 _amount) {
		position += _amount;
		
	}

public: // Properties
	RigidbodyType2D	bodyType; // A RigidbodyType2D used to define the behaviour of this body in the physics world.

	Vector2	velocity;	// The general velocity of this body in world space.
	Vector2 position;	// The position of the rigidbody.
	float rotation;		// The rotation of the rigidbody.

	float drag;			// 
	float angularDrag;	// 
	float inertia;		// 
	float mass;			// 
	float gravityScale;	// 

	bool useAutoMass = true;	// Does this body use physics to automatically calculate its body mass.

private:
	Collider2D* m_attachedCollider;

	float m_totalForce;		// The total amount of force that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
	float m_totalTorque;	// The total amount of torque that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
};

// +--------------------------------------------------------------------------------+
// |																				|
// | CONTACT POINT 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Details about a specific point of contact involved in a 2D physics collision.
// 
// A contact point describes a point of intersection between two Collider2D. 
// ContactPoint2D can only exist on Collider2D that are not set to be triggers as triggers do not define contact points.
//
struct ContactPoint2D {
	bool enabled;				// Indicates whether the collision response or reaction is enabled or disabled.

	Collider2D* collider;		// The incoming Collider2D involved in the collision with the otherCollider.
	Rigidbody2D* rigidbody;		// The incoming Rigidbody2D involved in the collision with the otherRigidbody.

	Collider2D* otherCollider;		// The other Collider2D involved in the collision with the collider.
	Rigidbody2D* otherRigidbody;	// The other Rigidbody2D involved in the collision with the rigidbody.

	Vector2 point;				// The point of contact between the two colliders in world space.
	Vector2 normal;				// Surface normal Get the contact point.
	Vector2 normalImpulse;		// Gets the impulse applied Get the contact point along the ContactPoint2D.normal.
	Vector2 relativeVelocity;	// Gets the relative velocity of the two colliders Get the contact point(Read Only).
	float separation;			// Gets the distance between the colliders Get the contact point.
	float tangentImpulse;		// Gets the impulse applied Get the contact point which is perpendicular to the ContactPoint2D.normal.
};

// +--------------------------------------------------------------------------------+
// |																				|
// | PHYSICS SCENE 2D																|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Represents a single instance of a 2D physics Scene.
// 
// A 2D physics Scene owns all 2D physics component objects added to it and can perform a simulation 
// of them as well as execute queries against them. 
// All of this functionality is isolated from any other physics Scene. 
// Using this, multiple independent physics Scenes can coexist.
//
class PhysicsScene2D {
public: // Constructors & Destructors
	inline PhysicsScene2D() = default;

public:
	//
	// Submit a new rigid body to the physics scene.
	// 
	// This body will be simulated in the next iteration.
	//
	inline bool Submit(Rigidbody2D* _body) {
		m_bodies.push_back(_body);
	}

	//
	// Advance the physics simulation by the given time.
	// 
	// @param _elapsedTime:
	//		The amount of time to advance the simulation.
	//		The smaller this is, the more accurate the simulation will be.
	//
	inline void Step(float _elapsedTime) {
		for (auto& body : m_bodies) {
			// If the game object is not active -> we skip updating this body.
			if (!body->gameObject->selfActive) continue;

			body->position += body->velocity * _elapsedTime;

			for (auto& otherBody : m_bodies) {
				if (otherBody == body) continue;

				body->ResolveCollision(*otherBody);
			}
		}
	}

private:
	std::vector<Rigidbody2D*> m_bodies;
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
public: // Typedef
	friend class LMKEngine;

public: // Constructors & Destructors
#pragma region Singleton
	Physics2D(const Physics2D&) = delete;

private:
	inline Physics2D() {
		// We need to create a default physics scene on initialization of Physics system.
		m_physicsScene = new PhysicsScene2D();
	}

public:
#pragma endregion

	inline ~Physics2D() {
		delete m_physicsScene;
		m_physicsScene = nullptr;
	}

public: // Static Functions
	//
	// Check for overlapping bounding circle.s
	//
	inline static bool OverlapBoundingCircle(Vector2 _centerA, float _radiusA, Vector2 _centerB, float _radiusB) {
		return Vector2::Distance(_centerA, _centerB) < _radiusA + _radiusB;
	}

private:
	//
	// Advance the physics simulation by _elapsedTime seconds.
	// 
	// This should only be called by the LMK Engine within the main FixedUpdate() loop.
	//
	inline static void Step(float _elapsedTime) {
		Instance.m_physicsScene->Step(_elapsedTime);
	}

public: // Accessors
	// 
	inline static float ContactOffset() {
		return Instance.m_contactOffset;
	}

	// Get the gravitational acceleration applied to the physics world.
	inline static Vector2 Gravity() {
		return Instance.m_gravity;
	}

public: // Static Properties

private: 
	static Physics2D Instance;

private: // Properties
	PhysicsScene2D* m_physicsScene = nullptr;

	float m_contactOffset = 0.01f;			// 
	Vector2 m_gravity = Vector2(0, -18.6f);	// Gravitational acceleration applied to the physics world.
};

Physics2D	Physics2D::Instance;
LMK_END

#endif // !_LMK_PHYSICS_2D_H_