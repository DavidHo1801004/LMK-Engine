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
		: m_verts({}), m_size(0) {
	}

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
	using vertices_storage = std::vector<Vector2>;
	using shapes_storage = std::vector<PhysicsShape2D>;

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
		m_shapes.push_back(PhysicsShape2D{
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
			Vector2{ _center.x - halfSize.x, _center.y + halfSize.y } *rotateMatrix,	// Top Left
			Vector2{ _center.x + halfSize.x, _center.y + halfSize.y } *rotateMatrix,	// Top Right
			Vector2{ _center.x + halfSize.x, _center.y - halfSize.y } *rotateMatrix,	// Bottom Right
			Vector2{ _center.x - halfSize.x, _center.y - halfSize.y } *rotateMatrix,	// Bottom Left
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
	inline bool DeleteShape(size_t _shapeIndex) {
		if (_shapeIndex < 0 || _shapeIndex >= m_shapes.size()) return false;

		auto& temp = m_shapes[_shapeIndex];
		m_shapes.erase(m_shapes.begin() + _shapeIndex);

		// Update shapes' vertexStartIndex.
		// All shapes that have was added after the given shape
		auto shapeIter = m_startVertIndexMap.lower_bound(temp.vertexStartIndex);

		for (auto iter = shapeIter; iter != m_startVertIndexMap.end(); iter++) {
			iter->second.vertexStartIndex -= temp.vertexCount;
		}

		m_startVertIndexMap.erase(shapeIter);

		return true;
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
	inline const PhysicsShape2D& GetShape(size_t _shapeIndex) const {
		LMK_CORE_ASSERT(_shapeIndex < m_shapes.size(),
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
		LMK_CORE_ASSERT(_shapeIndex < m_shapes.size(),
			"lmk::PhysicsShapeGroup2D: GetShapeVertex() - _shapeIndex: Index out of range.");
		LMK_CORE_ASSERT(_vertexIndex < m_shapes[_shapeIndex].vertexCount,
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
		LMK_CORE_ASSERT(_shapeIndex < m_shapes.size(),
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
	inline size_t GetShapeCount() const noexcept {
		return m_shapes.size();
	}

public: // Properties
	// Gets or sets a matrix that transforms the PhysicsShapeGroup2D vertices from local space into world space.
	// 
	// When retrieving a PhysicsShapeGroup2D via Rigidbody2D.GetShapes or Collider2D.GetShapes, 
	// this matrix will be set to the pose of the Rigidbody2D.
	// In case a Rigidbody2D is not available, the matrix it set to Matrix3x3.identity.
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

class Rigidbody2D;

//
// Base class for collider types used with 2D physics.
//
class Collider2D : public Behaviour {
public: // Constructors & Destructors
	//
	// Copy constructor.
	//
	inline Collider2D(const Collider2D& _other) {
		m_shapes->Add(*_other.m_shapes);

		RecalculateBounds();
	}

	//
	// Create a new Collider2D with no vertex data.
	//
	inline Collider2D() = default;

public: // Functions
	//
	// Gets all the PhysicsShape2D used by the Collider2D.
	// 
	// @return
	//		A PhysicsShapeGroup2D composed of all PhysicsShape2D retrieved from the Collider2D.
	//
	inline const PhysicsShapeGroup2D* GetShapes() const {
		return m_shapes.get();
	}

	//
	// Update the world space bounding box of this collider.
	//
	inline void RecalculateBounds() noexcept {
		Vector2 min = Vector2::positiveInfinity;
		Vector2 max = Vector2::negativeInfinity;

		for (size_t shapeID = 0; shapeID < m_shapes->GetShapeCount(); shapeID++) {
			const auto& shape = m_shapes->GetShape(shapeID);
			auto vertices = GetWorldVertices(shapeID);
			auto radiusVec = Vector2{ shape.radius, shape.radius };

			for (const auto& vertex : vertices) {
				min = Vector2::Min(min - radiusVec, vertex - radiusVec);
				max = Vector2::Max(max + radiusVec, vertex + radiusVec);
			}
		}

		m_bounds->SetMinMax(min, max);
	}

	//
	// Get all vertices of a given shape in world space, with Collider2D.scale applied.
	//
	[[nodiscard]]
	inline std::vector<Vector2> GetWorldVertices(size_t _shapeIdx) const noexcept {
		std::vector<Vector2> result;

		Matrix3x3 mtx;
		mtx.SetTRS(transform->GetPosition(), transform->GetRotation(), transform->GetLossyScale().Scale(scale));
		for (const auto& v : m_shapes->GetShapeVertices(_shapeIdx)) {
			result.push_back(mtx.MultiplyPoint(v));
		}

		return result;
	}

public: // Accessors
	// Get the bounding box of this collider.
	[[nodiscard]]
	inline const Bounds* GetBounds() const noexcept {
		return m_bounds.get();
	}

public: // Properties
	bool	isTrigger = false;	// Is this collider a trigger?
	float	bounciness = 0.0f;		// Get the bounciness used by the collider.
	float	friction = 0.5f;		// Get the friction used by the collider.
	float	density = 7.874f;	// The density of the collider used to calculate its mass (when auto mass is enabled).

	Vector2	offset = Vector2::zero;	// The local offset of this collider relative to the owning transform.
	Vector2	scale = Vector2::one;		// The size multiplier of this collider.

protected:
	// The original "model" of the Collider2D.
	std::shared_ptr<PhysicsShapeGroup2D> m_shapes = std::make_shared<PhysicsShapeGroup2D>();

	// The world space bounding area of the collider.
	std::shared_ptr<Bounds>	m_bounds = std::make_shared<Bounds>();

private:
	std::shared_ptr<Rigidbody2D> m_attachedRigidbody = nullptr;	// The Rigidbody2D attached to the Collider2D.
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
		: CircleCollider2D(1) {
	}
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
		: CapsuleCollider2D(false, Vector2(0.5f, 1)) {
	}
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
		: BoxCollider2D(Vector2::one, 0) {
	}
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
		: Collider2D() {
	}

public: // Functions
	//
	// 
	//
	inline void AddPath(const std::vector<Vector2>& _vertices) {
		m_shapes->AddPolygon(_vertices);
		RecalculateBounds();
	}

	//
	//
	//
	inline bool RemovePath(int _index) {
		bool result = m_shapes->DeleteShape(_index);
		RecalculateBounds();
		return result;
	}

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
	None = 0x0000,										// No constraints.
	FreezePositionX = 0x0001,							// Freeze motion along the X - axis.
	FreezePositionY = 0x0002,							// Freeze motion along the Y - axis.
	FreezeRotation = 0x0004,							// Freeze rotation along the Z - axis.
	FreezePosition = FreezePositionX | FreezePositionY,	// Freeze motion along the X - axis and Y - axis.
	FreezeAll = FreezeRotation | FreezePosition,		// Freeze rotation and motion along all axes.
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
// Rigidbody physics component for GameObjects.
// 
// Adding a Rigidbody2D component to an object puts it under the control of the physics engine.
//
class Rigidbody2D : public Component {
public: // Constructors & Destructors
	// TODO Auto register child colliders
	inline Rigidbody2D() = default;

public: // Functions
	//
	// Register a new collider to this body.
	//
	inline bool RegisterCollider(Collider2D* _collider) {
		m_attachedCollider = _collider;
		return true;
	}

	//
	// Apply an impulse to a body.
	// 
	// @param _impulse:
	//		A vector represents the direction and magnitude of the force.
	// 
	// @param _worldPoint:
	//		The impact point of the force in world space;
	//
	inline void ApplyImpulse(Vector2 _impulse, Vector2 _worldPoint) {
		auto r = _worldPoint - GetPosition();
		auto angularImpulse = LMK_RtoD(Vector2::Cross(r, _impulse));

		linearVelocity += _impulse * GetInvMass();
		angularVelocity += angularImpulse * GetInvInertia();
	}

protected:
	inline void OnAttached() override;

public: // Accessors
	// The total amount of force that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
	[[nodiscard]]
	inline float GetTotalForce() const {
		return m_totalForce;
	}

	// The total amount of torque that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
	[[nodiscard]]
	inline float GetTotalTorque() const {
		return m_totalTorque;
	}

	// Position of the body in world space. Same as Transform.GetPosition() if center of mass is default to (0, 0)
	[[nodiscard]]
	inline Vector2 GetPosition() const {
		return transform->GetPosition();
	}

	[[nodiscard]]
	inline Collider2D* GetCollider() const {
		return m_attachedCollider;
	}

	[[nodiscard]]
	inline float GetInvMass() const {
		if (bodyType == rgbody::Static) return 0;
		return mass == 0 ? 0 : 1 / mass;
	}

	[[nodiscard]]
	inline float GetInvInertia() const {
		if (bodyType == rgbody::Static) return 0;
		return inertia == 0 ? 0 : 1 / inertia;
	}

public: // Properties
	// A RigidbodyType2D used to define the behaviour of this body in the physics world.
	RigidbodyType2D	bodyType = rgbody::Dynamic;

	// Controls which degrees of freedom are allowed for the simulation of this Rigidbody2D.
	RigidbodyConstraints2D constrains = rgbody::None;

	// The linear velocity of the Rigidbody2D represents the rate of change over time of the Rigidbody2D position in world-units.
	Vector2	linearVelocity = Vector2::zero;

	// The linear damping of the Rigidbody2D linear velocity.
	float linearDamping = 0.f;

	// Angular velocity in degrees per second.
	float angularVelocity = 0;

	// The angular damping of the Rigidbody2D angular velocity.
	float angularDamping = 0.05f;

	// The Rigidbody's resistance to changes in angular velocity (rotation).
	float inertia = 1.f;

	// Mass of the Rigidbody.
	// The mass is given in arbitrary units, but the basic physical principles of mass apply.
	float mass = 1.f;

	// The degree to which this object is affected by gravity.
	float gravityScale = 1.f;

	// TODO add center of mass and update physics solver.


private:
	// TODO handle multi colliders
	Collider2D* m_attachedCollider = nullptr;

	// The total amount of force that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
	float m_totalForce = 0;

	// The total amount of torque that has been explicitly applied to this Rigidbody2D since the last physics simulation step.
	float m_totalTorque = 0;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | CONSTRAINS																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
//
//
class Constraint {
private:
	friend class PhysicsScene2D;

protected: // Constructors
	inline Constraint(Rigidbody2D* _A, Rigidbody2D* _B) {
		bodyA = _A;
		bodyB = _B;
	}

public: // Functions
	//
	// Caled each physics step before solving bodies.
	//
	virtual void Update() = 0;

	//
	// @param _dTime:
	//		Number of seconds between the last physics step and this one.
	// 
	// @param _baumgarteFactor:
	//		Baumgarte stabilization factor to avoid constraints driffting.
	//
	virtual void Solve(float _dTime, float _baumgarteFactor) = 0;

protected: // Properties
	Rigidbody2D* bodyA;
	Rigidbody2D* bodyB;
};

//
// 
//
class EqualityConstraint: public Constraint {
public:
	EqualityConstraint(Rigidbody2D* _A, Rigidbody2D* _B, Vector2 _worldP) 
		: Constraint(_A, _B) {
		m_localA = _A->transform->InverseTransformPoint(_worldP);
		m_localB = _B->transform->InverseTransformPoint(_worldP);
	}

public:
	inline void Update() override {
		m_worldA = bodyA->transform->TransformPoint(m_localA);
		m_worldB = bodyB->transform->TransformPoint(m_localB);
		m_rA = m_worldA - bodyA->GetPosition();
		m_rB = m_worldB - bodyB->GetPosition();

		m_mA = bodyA->GetInvMass();
		m_mB = bodyB->GetInvMass();
		m_iA = bodyA->GetInvInertia();
		m_iB = bodyB->GetInvInertia();
	}

	inline void Solve(float _dTime, float _baumgarteFactor) override {
		Vector2 velA = bodyA->linearVelocity + Vector2::Perpendicular(m_rA) * LMK_DtoR(bodyA->angularVelocity);
		Vector2 velB = bodyB->linearVelocity + Vector2::Perpendicular(m_rB) * LMK_DtoR(bodyB->angularVelocity);
		Vector2 relVel = velB - velA;
		Vector2 relPos = m_worldB - m_worldA;

		float K[2][2]{};
		K[0][0] = m_mA + m_mB + m_rA.y * m_rA.y * m_iA + m_rB.y * m_rB.y * m_iB;
		K[0][1] = -m_rA.y * m_rA.x * m_iA - m_rB.y * m_rB.x * m_iB;
		K[1][0] = K[0][1];
		K[1][1] = m_mA + m_mB + m_rA.x * m_rA.x * m_iA + m_rB.x * m_rB.x * m_iB;

		// Avoid drifting constraints
		Vector2 bias = relPos * (_baumgarteFactor / _dTime);
		Vector2 cwb = -(relVel + bias);

		// Solve using Cramer's rule
		float det = K[0][0] * K[1][1] - K[0][1] * K[1][0];
		float invDet = det != 0 ? 1.f / det : 0;
		Vector2 impulse(invDet * (K[1][1] * cwb.x - K[0][1] * cwb.y), invDet * (K[0][0] * cwb.y - K[1][0] * cwb.x));

		bodyA->ApplyImpulse(-impulse, m_worldA);
		bodyB->ApplyImpulse(impulse, m_worldB);
	}

public:
	inline Vector2 GetWorldA() const {
		return m_worldA;
	}

	inline Vector2 GetWorldB() const {
		return m_worldB;
	}

private: // Properties
	Vector2 m_worldA, m_worldB;
	Vector2 m_localA, m_localB;
	Vector2 m_rA, m_rB;

	float m_mA, m_mB; // inverse mass
	float m_iA, m_iB; // inverse inertia
};

//
// 
//
class ContactConstraint : public Constraint {
public:
	ContactConstraint(Rigidbody2D* _A, Rigidbody2D* _B, Vector2 _worldP, Vector2 _normal, float _penetration)
		: Constraint(_A, _B) {
		m_mA = bodyA->GetInvMass();
		m_mB = bodyB->GetInvMass();
		m_iA = bodyA->GetInvInertia();
		m_iB = bodyB->GetInvInertia();

		m_friction = std::sqrt(bodyA->GetCollider()->friction * bodyB->GetCollider()->friction);
		SetCollisionData(_worldP, _normal, _penetration);

		std::cout << "Added: " << bodyA->gameObject->name << " -- " << bodyB->gameObject->name << "\n";
	}

	~ContactConstraint() {
		std::cout << "Deleted: " << bodyA->gameObject->name << " -- " << bodyB->gameObject->name << "\n";
	}

public:
	inline void SetCollisionData(Vector2 _worldP, Vector2 _normal, float _penetration) {
		m_normal = _normal;
		m_tangent = Vector2(_normal.y, -_normal.x); // rotate 90 degree cw
		m_penetration = _penetration;
		m_localA = bodyA->transform->InverseTransformPoint(_worldP);
		m_localB = bodyB->transform->InverseTransformPoint(_worldP);
	}

	inline void Update() override {
		m_worldA = bodyA->transform->TransformPoint(m_localA);
		m_worldB = bodyB->transform->TransformPoint(m_localB);
		m_rA = m_worldA - bodyA->GetPosition();
		m_rB = m_worldB - bodyB->GetPosition();

		m_mA = bodyA->GetInvMass();
		m_mB = bodyB->GetInvMass();
		m_iA = bodyA->GetInvInertia();
		m_iB = bodyB->GetInvInertia();

		// warm starting
		auto normalImpulse = m_normal * m_accNormalLambda;
		auto tangentImpulse = m_tangent * m_accFrictionLambda;
		auto totalImpulse = normalImpulse + tangentImpulse;
		bodyA->ApplyImpulse(-totalImpulse, m_worldA);
		bodyB->ApplyImpulse(totalImpulse, m_worldB);
	}

	inline void Solve(float _dTime, float _baumgarteFactor) override {
		SolveContact(_dTime, _baumgarteFactor);
		SolveFriction();
	}

private:
	inline void SolveContact(float _dTime, float _baumgarteFactor) {
		Vector2 velA = bodyA->linearVelocity + Vector2::Perpendicular(m_rA) * LMK_DtoR(bodyA->angularVelocity);
		Vector2 velB = bodyB->linearVelocity + Vector2::Perpendicular(m_rB) * LMK_DtoR(bodyB->angularVelocity);
		Vector2 relVel = velB - velA;
		float cp = Vector2::Dot(relVel, m_normal);

		float rnA = Vector2::Cross(m_rA, m_normal);
		float rnB = Vector2::Cross(m_rB, m_normal);
		float eMass = m_mA + m_mB + rnA * rnA * m_iA + rnB * rnB * m_iB;
		if (eMass < 1e-6f) return;

		float allowedPenetration = 1e-4f;
		float separation = LMK_Min(0, -m_penetration + allowedPenetration);
		float vBias = (_baumgarteFactor / _dTime) * separation;

		// normal impulse
		float lambda = -(cp + vBias) / eMass;
		float oldAcc = m_accNormalLambda;
		m_accNormalLambda = LMK_Max(m_accNormalLambda + lambda, 0);
		lambda = m_accNormalLambda - oldAcc;

		if (lambda == 0) return;

		Vector2 impulse = m_normal * lambda;
		bodyA->ApplyImpulse(-impulse, m_worldA);
		bodyB->ApplyImpulse(impulse, m_worldB);
	}

	inline void SolveFriction() {
		if (m_friction <= 0) return;
		Vector2 velA = bodyA->linearVelocity + Vector2::Perpendicular(m_rA) * LMK_DtoR(bodyA->angularVelocity);
		Vector2 velB = bodyB->linearVelocity + Vector2::Perpendicular(m_rB) * LMK_DtoR(bodyB->angularVelocity);
		Vector2 relVel = velB - velA;
		float cp = Vector2::Dot(relVel, m_tangent);

		float rtA = Vector2::Cross(m_rA, m_tangent);
		float rtB = Vector2::Cross(m_rB, m_tangent);
		float eMassTangent = m_mA + m_mB + rtA * rtA * m_iA + rtB * rtB * m_iB;
		if (eMassTangent < 1e-6f) return;

		float lambda = -cp / eMassTangent;
		float maxFriction = m_friction * m_accNormalLambda;
		float oldAcc = m_accFrictionLambda;
		m_accFrictionLambda = LMK_Max(-maxFriction, LMK_Min(oldAcc + lambda, maxFriction));
		lambda = m_accFrictionLambda - oldAcc;

		Vector2 frictionImpulse = m_tangent * lambda;
		bodyA->ApplyImpulse(-frictionImpulse, m_worldA);
		bodyB->ApplyImpulse(frictionImpulse, m_worldB);
	}

public:
	inline Vector2 GetWorldContactPointA() const {
		return m_worldA;
	}

	inline Vector2 GetWorldContactPointB() const {
		return m_worldB;
	}

public: // Properties
	float m_accNormalLambda = 0;
	float m_accFrictionLambda = 0;

	Vector2 m_normal, m_tangent;
	float m_penetration;
	float m_friction;

	Vector2 m_worldA, m_worldB;
	Vector2 m_localA, m_localB;
	Vector2 m_rA, m_rB;

	float m_mA, m_mB; // inverse mass
	float m_iA, m_iB; // inverse inertia
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
public:
	friend class Physics2D;

public: // Constructors & Destructors
	inline PhysicsScene2D() = default;

public:
	//
	// Submit a new rigid body to the physics scene.
	// 
	// This body will be simulated in the next iteration.
	//
	inline bool Register(Rigidbody2D* _body) {
		m_bodies.push_back(_body);

		return true;
	}

	//
	// Submit a new constraint to the physics scene.
	// 
	inline bool Register(Constraint* _constraint) {
		m_constraints.push_back(_constraint);

		return true;
	}

private:
	//
	// Advance the physics simulation by the given time.
	// 
	// @param _elapsedTime:
	//		The number of seconds to advance the simulation.
	//		The smaller this is, the more accurate the simulation will be.
	//
	inline void Step(float _dTime);

	inline void SolveConstraints(float _dTime) {
		for (auto& c : m_constraints) {
			c->Update();
		}

		for (size_t i = 0; i < m_constraintSolverIterations; i++) {
			for (auto& c : m_constraints) {
				c->Solve(_dTime, m_baumgarteFactor);
			}
		}
	}

	inline void FinalizeBodies(float _dTime) {
		for (size_t i = 0; i < m_bodies.size(); i++) {
			auto body = m_bodies[i];

			body->transform->Translate(body->linearVelocity * _dTime);
			body->transform->Rotate(body->angularVelocity * _dTime);
		}
	}

public:
	inline std::vector<ContactConstraint*> GetContacts() {
		std::vector<ContactConstraint*> result;
		for (auto& contact : m_contactCache) {
			result.push_back(contact.second);
		}

		return result;
	}

private: // Properties
	std::vector<Rigidbody2D*> m_bodies;
	std::vector<Constraint*> m_constraints;
	std::unordered_map<uint64_t, ContactConstraint*> m_contactCache;

	size_t m_bodySolverIterations = 8;
	size_t m_constraintSolverIterations = 10;
	float m_baumgarteFactor = 0.05f;
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
	friend class PhysicsScene2D;

	// Contains projection data of a vector onto a plane.
	struct ProjectionResult {
		float minDis;
		float maxDis;
	};

	// Contains a list of vertices and radius to presents rounded shapes.
	struct Polygon {
		inline explicit Polygon(float _radius, const std::vector<Vector2>& _vertices)
			: radius(_radius), vertices(_vertices) {
		}

		float radius;					// The edge radius value of the polygon.
		std::vector<Vector2> vertices;	// The vertices of the polygon.

		inline int Count() const {
			return vertices.size();
		}
	};

	// Represents a vertex of a polygon
	struct Vertex {
		Vector2 position;

		// Index in polygon
		size_t id;
	};

	// Represents an edge/line segment
	struct Segment {
		Vertex v1; // first vertex
		Vertex v2; // second vertex

		Vector2 GetForward() const {
			return (v2.position - v1.position).GetNormalized();
		}

		Vector2 GetBackward() const {
			return (v1.position - v2.position).GetNormalized();
		}

		Vector2 GetMidPoint() const {
			return v2.position + (v1.position - v2.position) / 2.f;
		}

		inline Vertex& operator[](size_t _index) {
			LMK_CORE_ASSERT(_index <= 1,
				"lmk::Physics2D::Segment: operator[]() - Index out of range. (index in range [0..1] expected)")

			return _index == 0 ? v1 : v2;
		}

		inline const Vertex& operator[](size_t _index) const {
			LMK_CORE_ASSERT(_index <= 1,
				"lmk::Physics2D::Segment: operator[]() - Index out of range. (index in range [0..1] expected)")

			return _index == 0 ? v1 : v2;
		}
	};

	// Contains all the detail related to the geometry and dynamics of the contact. 
	// Use ManifoldPoint.totalNormalImpulse to determine if there was an interaction during the time step.
	struct ManifoldPoint {
		// Uniquely identifies a contact point between two shapes.
		int id;

		// Location of the contact point relative to shapeA's origin in world space.
		Vector2 anchorA;

		// Location of the contact point relative to shapeB's origin in world space.
		Vector2 anchorB;

		// Location of the contact point in world space. Subject to precision loss at large coordinates. 
		// This point lags behind when contact recycling is used. 
		// Preference should be to use anchorA and/or anchorB for game logic. This is also known as the "clip" point.
		Vector2 point;

		// The impulse along the manifold normal vector.
		float normalImpulse;

		// Relative normal velocity pre - solve. Used for hit events.
		// If the normal impulse is zero then there was no hit.
		// Negative means shapes are approaching.
		float normalVelocity;

		// The friction impulse.
		float tangentImpulse;

		// The total normal impulse applied across sub-stepping and restitution. 
		// This includes the warm starting impulse, the sub-step delta impulse, and the restitution impulse. 
		// This can be used to identify speculative contact points that had an interaction during the simulation step.
		float totalNormalImpulse;

		// Did this contact point exist the previous step?
		bool persisted;

		// The separation of the contact point, negative if penetrating.
		float separation;

		// Is the contact point speculative i.e. not currently interacting?
		bool speculative;
	};

	// Fixed-sized manifold point array.
	struct ManifoldPointArray {
		std::shared_ptr<ManifoldPoint> contactInfo0 = nullptr;
		std::shared_ptr<ManifoldPoint> contactInfo1 = nullptr;

		inline std::shared_ptr<ManifoldPoint>& operator[](size_t _index) {
			LMK_CORE_ASSERT(_index <= 1,
				"lmk::Physics2D::ManifoldPointArray: operator[]() - Index out of range. (index in range [0..1] expected)")

			return _index == 0 ? contactInfo0 : contactInfo1;
		}
	};

	// A contact manifold describes the contact points between colliding shapes. 
	struct ContactManifold {
		// The unit normal vector in world space, points from shape A to bodyB
		Vector2 normal;

		// The number of manifold points available, in the range [0, 2].
		int pointCount;

		// The manifold points, up to two are possible.
		ManifoldPointArray points;

		// Angular impulse applied for rolling resistance (N " m " s = kg * m^2 / s).
		float rollingImpulse;

		inline std::shared_ptr<ManifoldPoint>& operator[](size_t _index) {
			return points[_index];
		}
	};

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
	// Check for overlapping bounding circles.
	//
	inline static bool OverlapBoundingCircle(Vector2 _centerA, float _radiusA, Vector2 _centerB, float _radiusB) {
		return Vector2::Distance(_centerA, _centerB) < _radiusA + _radiusB;
	}

public:
	//
	// Advance the physics simulation by _elapsedTime seconds.
	// 
	// This should only be called by the LMK Engine within the main FixedUpdate() loop.
	//
	inline static void Step(float _elapsedTime) {
		Instance.m_physicsScene->Step(_elapsedTime);
	}

	//
	// Helper function for retrieving polygons from a collider's shape group.
	//
	inline static void PopulatePolygonsFromCollider(const Collider2D& _collider, std::vector<Polygon>& _polygons) {
		auto shapes = _collider.GetShapes();

		for (size_t i = 0; i < shapes->GetShapeCount(); i++) {
			auto verts = _collider.GetWorldVertices(i);
			_polygons.emplace_back(shapes->GetShape(i).radius, verts);
		}
	}

	//
	// Standard implementation of SAT (Separating Axis Theorem) between 2 colliders.
	//
	[[deprecated("Use CollisionDetection_GJK instead")]]
	inline static ColliderDistance2D CollisionDetection_SAT(const Collider2D& _colliderA, const Collider2D& _colliderB) {
		std::vector<Polygon> polysA, polysB;
		PopulatePolygonsFromCollider(_colliderA, polysA);
		PopulatePolygonsFromCollider(_colliderB, polysB);

		ColliderDistance2D best;
		best.distance = FLT_MAX;
		best.isValid = false;

		for (auto& polyA : polysA) {
			for (auto& polyB : polysB) {
				auto result = SAT(polyA, polyB);

				if (!result.isValid) continue;

				// If separated → early out (optional)
				if (!result.isOverlapped)
					return result;

				if (!best.isValid || result.distance > best.distance) {
					best = result;
					best.isValid = true;
				}
			}
		}

		return best;
	}

	//
	// Standard implementation of GJK (Gilbert-Johnson-Keerthi) algorithm for collision detection
	// between 2 colliders.
	// 
	// @return
	//		A contact manifold containing contact data.
	//
	inline static std::shared_ptr<ContactManifold> CollisionDetection_GJK(
		const Collider2D& _colliderA, 
		const Collider2D& _colliderB,
		size_t _idColA,
		size_t _idColB) {

		if (!_colliderA.GetBounds()->Intersects(*(_colliderB.GetBounds()))) return nullptr;

		std::vector<Polygon> polysA, polysB;
		PopulatePolygonsFromCollider(_colliderA, polysA);
		PopulatePolygonsFromCollider(_colliderB, polysB);

		// Test all shape pairs
		impl::Simplex simplex;
		for (size_t i = 0; i < polysA.size(); i++) {
			auto& polyA = polysA[i];

			for (size_t j = 0; j < polysA.size(); j++) {
				auto& polyB = polysB[i];

				if (!GJKCore(polyA, polyB, simplex)) continue;

				auto epa = EPACore(polyA, polyB, simplex, 32);
				auto manifold = GenerateContactManifold(
					_colliderA, _colliderB, polyA, polyB, epa.normal,
					_idColA, _idColB, i, j);

				// TODO Handling multi pair collision
				if (manifold)
					return manifold;
			}
		}

		return nullptr;
	}

public:
#pragma region SAT Algorithm
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
	// 
	//
	static ColliderDistance2D SAT(const Polygon& _A, const Polygon& _B) {
		ColliderDistance2D result;
		result.distance = FLT_MAX;
		result.isValid = true;

		auto testAxes = [&](const Polygon& _P1, const Polygon& _P2) -> bool {
			size_t n = _P1.vertices.size();

			for (size_t cur = n - 1, next = 0; next < n; cur = next++) {
				Vector2 edge = _P1.vertices[next] - _P1.vertices[cur];
				Vector2 axis = Vector2::Perpendicular(edge).GetNormalized();

				auto proj1 = ProjectPolygon(_P1, axis);
				auto proj2 = ProjectPolygon(_B, axis);

				float overlap = std::min(proj1.maxDis, proj2.maxDis) -
					std::max(proj1.minDis, proj2.minDis);

				// Separation -> positive distance
				if (overlap < 0) {
					result.distance = overlap; // negative overlap means separation
					result.normal = axis;
					result.isOverlapped = false;
					return false;
				}

				// Track minimum penetration
				if (overlap < -result.distance || result.distance > 0) {
					result.distance = -overlap; // negative = penetration
					result.normal = axis;
				}
			}

			return true;
			};

		if (!testAxes(_A, _B)) return result;
		if (!testAxes(_B, _A)) return result;

		result.isOverlapped = true;
		return result;
	}
#pragma endregion

#pragma region GJK Algorithm
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
	inline static Vertex FindFurthestVertex(const Polygon& _polygon, Vector2 _direction) noexcept {
		Vector2 maxPoint;
		size_t maxId = 0;
		float maxDis = -INFINITY;

		// Find furthest projection of vertices on _direction.
		for (size_t i = 0; i < _polygon.Count(); i++) {
			auto& vert = _polygon.vertices[i];
			float distance = Vector2::Dot(vert, _direction);
			if (distance > maxDis) {
				maxDis = distance;
				maxPoint = vert;
				maxId = i;
			}
		}
		// Add physics shape radius to the result vertex.
		// This generalizes the function for all non-polygon shapes.
		maxPoint += _direction.GetNormalized() * _polygon.radius;

		return Vertex{ maxPoint, maxId };
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
	inline static Vector2 GJKSupport(const Polygon& _polygonA, const Polygon& _polygonB, Vector2 _direction) noexcept {
		auto vertexA = FindFurthestVertex(_polygonA, _direction);
		auto vertexB = FindFurthestVertex(_polygonB, -_direction);
		return vertexA.position - vertexB.position;
	}

	//
	// Implementation of GJK (Gilbert-Johnson-Keerthi) algorithm for overlap detection
	// between two polygons.
	// 
	// @param _resultSimplex:
	//		The result simplex if the colliders overlapped.
	//
	[[nodiscard]]
	inline static bool GJKCore(const Polygon& _polygonA, const Polygon& _polygonB, impl::Simplex& _resultSimplex) {
		// For a detailed explanation of the Gilbert�Johnson�Keerthi algorithm, see:
		// https://medium.com/@mbayburt/walkthrough-of-the-gjk-collision-detection-algorithm-80823ef5c774
		// and
		// https://winter.dev/articles/gjk-algorithm

		// Get initial support point in any direction (vector (1, 0) in this case)
		Vector2 support = GJKSupport(_polygonA, _polygonB, Vector2::right);

		impl::Simplex simplex;
		simplex.push_front(support);

		// Set the next direction towards the origin.
		Vector2 direction = -support;

		while (true) {
			support = GJKSupport(_polygonA, _polygonB, direction);

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

	//
	// Case handler for 2D simplex.
	//
	[[nodiscard]]
	inline static bool GJK_HandleSimplex(impl::Simplex& _simplex, Vector2& _direction) {
		if (_simplex.size() == 2)
			return GJK_LineCase(_simplex, _direction);
		else
			return GJK_TriangleCase(_simplex, _direction);
	}

	//
	// Standard solver for GJK line case.
	//
	[[nodiscard]]
	inline static bool GJK_LineCase(impl::Simplex& _simplex, Vector2& _direction) {
		Vector2 a = _simplex[0];
		Vector2 b = _simplex[1];

		Vector2 ab = b - a;
		Vector2 ao = -a;

		if (Vector2::Dot(ab, ao) > 0) {
			// We use triple product to calculate the correct normal orientation.
			_direction = Vector3::Cross(Vector3::Cross(ab, ao), ab);

			if (_direction.GetSqrMagnitude() < 1e-6f) {
				_direction = Vector2::Perpendicular(ab);
			}
		}
		else {
			_simplex = { a };
			_direction = ao;
		}

		return false;
	}

	//
	// Standard solver for GJK triangle case.
	//
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
#pragma endregion

#pragma region EPA Resolution
	//
	// EPA (Expanding Polytope Algorithm) collision resolution between 2 polygons.
	//
	[[nodiscard]]
	inline static ColliderDistance2D EPACore(const Polygon& _polygonA, const Polygon& _polygonB, const impl::Simplex& _simplex, int _maxIterations) noexcept {
		constexpr float ep = 1e-4f; // epsilon
		constexpr float epd = 1e-6f; // duplicated epsilon

		size_t minIndex = 0;
		float minDistance = INFINITY;
		Vector2 minNormal;
		std::vector<Vector2> polytope = _simplex;

		auto SamePoint = [&](const Vector2& a, const Vector2& b) {
			return (a - b).GetSqrMagnitude() < epd * epd;
			};

		// To ensure CCW winding
		auto SignedArea = [&](const std::vector<Vector2>& pts) {
			float area = 0.0f;
			for (size_t i = 0; i < pts.size(); i++) {
				const Vector2& p = pts[i];
				const Vector2& q = pts[(i + 1) % pts.size()];
				area += (p.x * q.y - q.x * p.y);
			}
			return area * 0.5f;
			};

		if (SignedArea(polytope) < 0.0f) {
			std::reverse(polytope.begin(), polytope.end());
		}

		// --- Main EPA loop ---
		for (int iter = 0; iter < _maxIterations; iter++) {

			// Find closest edge
			float minDistance = INFINITY;
			Vector2 bestNormal;
			int bestIndex = 0;

			for (size_t i = 0; i < polytope.size(); i++) {
				size_t j = (i + 1) % polytope.size();

				const Vector2& a = polytope[i];
				const Vector2& b = polytope[j];

				Vector2 edge = b - a;

				// Degenerate edge check
				if (edge.GetSqrMagnitude() < epd) continue;

				// Outward normal for CCW polygon
				Vector2 normal(edge.y, -edge.x);
				normal = normal.GetNormalized();

				float distance = Vector2::Dot(a, normal);

				if (distance < minDistance) {
					minDistance = distance;
					bestNormal = normal;
					bestIndex = (int)j;
				}
			}

			// Get support point
			Vector2 support = GJKSupport(_polygonA, _polygonB, bestNormal);
			float supportDistance = Vector2::Dot(support, bestNormal);

			// Convergence check
			if (supportDistance - minDistance < ep) {
				return { minDistance, bestNormal };
			}

			// Duplicate check
			bool isDuplicate = false;
			for (const auto& p : polytope) {
				if (SamePoint(p, support)) {
					isDuplicate = true;
					break;
				}
			}

			if (isDuplicate) {
				// Can't expand further safely
				return { minDistance, bestNormal };
			}

			// Insert new point
			polytope.insert(polytope.begin() + bestIndex, support);
		}

		// Fallback (best known edge)
		minDistance = INFINITY;
		Vector2 bestNormal;

		for (size_t i = 0; i < polytope.size(); i++) {
			size_t j = (i + 1) % polytope.size();

			const Vector2& a = polytope[i];
			const Vector2& b = polytope[j];

			Vector2 edge = b - a;
			if (edge.GetSqrMagnitude() < 1e-6f) continue;

			Vector2 normal(edge.y, -edge.x);
			normal = normal.GetNormalized();

			float distance = Vector2::Dot(a, normal);

			if (distance < minDistance) {
				minDistance = distance;
				bestNormal = normal;
			}
		}

		return { minDistance, bestNormal };
	}
#pragma endregion

#pragma region Contacts Manifold
	//
	// Get the furthest vertex in a given direction, and returns an adjacent edge that 
	// is the most perpendicular to the given direction.
	//
	inline static Segment GetBestEdge(const Polygon& _poly, const Vector2& _dir) {
		auto support = FindFurthestVertex(_poly, _dir);

		// Get adjacent vertices
		Vector2 p = support.position;

		size_t prevId = (support.id - 1 + _poly.Count()) % _poly.Count();
		size_t nextId = (support.id + 1) % _poly.Count();
		Vector2 prevP = _poly.vertices[prevId];
		Vector2 nextP = _poly.vertices[nextId];

		// Pick most perpendicular edge to normal
		Vector2 prevV = (p - prevP).GetNormalized();
		Vector2 nextV = (nextP - p).GetNormalized();

		if (std::abs(Vector2::Dot(prevV, _dir)) > std::abs(Vector2::Dot(nextV, _dir))) {
			return { support, Vertex{ nextP, nextId } };
		}
		else {
			return { Vertex{ prevP, prevId }, support };
		}
	}

	//
	// Clip a line segment to only include points inside a half-space
	// define from a given normal and offset.
	//
	inline static int ClipSegmentToLine(Segment& _segment, const Vector2& _normal, float _offset) {
		Segment out = _segment;
		int numOut = 0;

		float dist0 = Vector2::Dot(_segment.v1.position, _normal) - _offset;
		float dist1 = Vector2::Dot(_segment.v2.position, _normal) - _offset;

		// Keep points inside
		if (dist0 <= 0) out[numOut++].position = _segment.v1.position;
		if (dist1 <= 0) out[numOut++].position = _segment.v2.position;

		// If segment crosses plane, compute intersection
		if (dist0 * dist1 < 0)
		{
			float t = dist0 / (dist0 - dist1);
			out[numOut++].position = _segment.v1.position + t * (_segment.v2.position - _segment.v1.position);
		}

		_segment.v1 = out.v1;
		_segment.v2 = out.v2;

		return numOut;
	}

	//
	// Core function to generate contact manifold with a given contact normal.
	//
	inline static std::shared_ptr<ContactManifold> GenerateContactManifold(
		const Collider2D& _colliderA,
		const Collider2D& _colliderB,
		const Polygon& _polygonA, 
		const Polygon& _polygonB, 
		const Vector2& _inputNormal,
		size_t _idColA,
		size_t _idColB,
		size_t _idPolyA,
		size_t _idPolyB) {

		std::shared_ptr<ContactManifold> contactManifold = std::make_shared<ContactManifold>();
		contactManifold->normal = _inputNormal;

		Vector2 normal = _inputNormal.GetNormalized();

		Segment edgeA = GetBestEdge(_polygonA, normal);
		Segment edgeB = GetBestEdge(_polygonB, -normal);

		// Ref edge is the more perpendicular edge to contact normal.
		float dotA = std::abs(Vector2::Dot(edgeA.GetForward(), normal));
		float dotB = std::abs(Vector2::Dot(edgeB.GetForward(), normal));
		Segment *refEdge, *incEdge;
		size_t refColID, incColID, refPolyID, incPolyID; // used for generate feature id.
		if (dotA <= dotB)
		{
			refEdge = &edgeA;
			incEdge = &edgeB;
			refColID = _idColA;
			incColID = _idColB;
			refPolyID = _idPolyA;
			incPolyID = _idPolyB;
		}
		else
		{
			refEdge = &edgeB;
			incEdge = &edgeA;
			refColID = _idColB;
			incColID = _idColA;
			refPolyID = _idPolyB;
			incPolyID = _idPolyA;
		}

		// Side planes to clip off irrelevant segments
		Vector2 sideNormal1 = refEdge->GetForward();
		Vector2 sideNormal2 = refEdge->GetBackward();
		float offset1 = Vector2::Dot(refEdge->v2.position, sideNormal1);
		float offset2 = Vector2::Dot(refEdge->v1.position, sideNormal2);

		if (ClipSegmentToLine(*incEdge, sideNormal1, offset1) < 2) return contactManifold;
		if (ClipSegmentToLine(*incEdge, sideNormal2, offset2) < 2) return contactManifold;

		// Face edge reference to define which contact point is penetrating
		Vector2 faceNormal = Vector2::Perpendicular(refEdge->GetForward());
		float refOffset = Vector2::Dot(refEdge->v1.position, faceNormal);

		int contactCount = 0;
		for (int i = 0; i < 2; ++i)	
		{
			float separation = Vector2::Dot((*incEdge)[i].position, faceNormal) - refOffset;

			if (separation <= 1e-6f)
			{
				// feature IDs combine body/collider and shape IDs with vertex indices
				auto featureIds = 
					refColID << 49 | 
					incColID << 33 |
					refPolyID << 25 |
					incPolyID << 17 |
					refEdge->v1.id << 9 |
					incEdge->v1.id << 1 |
					i;

				(*contactManifold)[contactCount] = std::make_shared<ManifoldPoint>();
				(*contactManifold)[contactCount]->id = featureIds;
				(*contactManifold)[contactCount]->point = (*incEdge)[i].position;
				(*contactManifold)[contactCount]->separation = separation;
				contactCount++;
			}
		}

		contactManifold->normal = normal;
		contactManifold->pointCount = contactCount;

		return contactManifold;
	}
#pragma endregion

public: // Accessors
	// Get the gravitational acceleration applied to the physics world.
	inline static Vector2 GetGravity() {
		return Instance.m_gravity;
	}

	inline static PhysicsScene2D* GetScene() {
		return Instance.m_physicsScene;
	}

private: // Static Properties
	static Physics2D Instance;

private: // Properties
	PhysicsScene2D* m_physicsScene = nullptr;

	float m_contactOffset = 0.01f;			// 
	Vector2 m_gravity = Vector2(0, -18.6f);	// Gravitational acceleration applied to the physics world.
};

Physics2D	Physics2D::Instance;


// 0========================================================================================================0
// |																										|
// | SEPERATE IMPLEMENTATION																				|
// |																										|
// 0========================================================================================================0

inline void Rigidbody2D::OnAttached() {
	RegisterCollider(gameObject->GetComponent<Collider2D>());

	Physics2D::GetScene()->Register(this);
}

inline void PhysicsScene2D::Step(float _dTime)
{
	// Prepare
	for (auto& body : m_bodies) {
		body->GetCollider()->RecalculateBounds();

		if (body->bodyType != RigidbodyType2D::Static)
			body->linearVelocity += Physics2D::GetGravity() * body->gravityScale * _dTime;

		body->linearVelocity *= (1.0f - body->linearDamping * _dTime);
		body->angularVelocity *= (1.0f - body->angularDamping * _dTime);
	}

	// Generate contacts
	std::unordered_set<uint64_t> activeContacts;

	for (size_t i = 0; i < m_bodies.size(); i++) {
		auto& current = m_bodies[i];
		auto currentCol = current->GetCollider();

		if (currentCol->isTrigger) continue;

		for (size_t j = i + 1; j < m_bodies.size(); j++) {
			auto& other = m_bodies[j];
			auto otherCol = other->GetCollider();

			if (otherCol->isTrigger) continue;

			auto manifold = Physics2D::CollisionDetection_GJK(*currentCol, *otherCol, i, j);

			if (!manifold) continue;

			for (size_t c = 0; c < manifold->pointCount; c++) {
				auto& mp = (*manifold)[c];

				activeContacts.insert(mp->id);

				auto it = m_contactCache.find(mp->id);
				if (it != m_contactCache.end()) {
					it->second->SetCollisionData(
						mp->point,
						manifold->normal,
						-mp->separation
					);
				}
				else {
					ContactConstraint* contact = new ContactConstraint(
						current, other,
						mp->point,
						manifold->normal,
						-mp->separation
					);

					m_contactCache[mp->id] = contact;
					Register(contact);
				}
			}
		}
	}

	// Remove dead contacts
	for (auto it = m_contactCache.begin(); it != m_contactCache.end(); ) {
		if (activeContacts.find(it->first) == activeContacts.end()) {
			m_constraints.erase(
				std::remove(m_constraints.begin(), m_constraints.end(), it->second), 
				m_constraints.end());

			delete it->second;
			it = m_contactCache.erase(it);
		}
		else {
			++it;
		}
	}
	
	SolveConstraints(_dTime);

	FinalizeBodies(_dTime);
}

LMK_END

#endif // !_LMK_PHYSICS_2D_H_