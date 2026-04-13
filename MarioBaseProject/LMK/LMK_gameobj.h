#pragma once
#ifndef _LMK_GAMEOBJ_H_
#define _LMK_GAMEOBJ_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"

LMK_BEGIN
//
// IMPORTANT:
// For this header only, we will seperate the declaration and implemetation of classes.
// 
// The reason for this is the original design of the Game Scene system was by using an ECS.
// 
// But due to the fact that this project can not contain any vendors, and I don't have the 
// required knowledge to build an ECS from scratch, I had to redesign the system in the last 
// minute, therefore came the problem of cyclic calling order.
// 
// I know that this is a bad practice for many reasons (not the seperation of declaration and 
// implementation but the fact that this is the only header that have this), but this
// was the only choice to resolve a bigger problem (without rewriting the whole engine).
//

// Forward declarations
class Component;

class Behaviour;

class MonoBehaviour;

class Transform;

class GameObject;

class Scene;

// 0========================================================================================================0
// |																										|
// | DECLARATIONS																							|
// |																										|
// 0========================================================================================================0

// +--------------------------------------------------------------------------------+
// |					 															|
// | COMPONENT			 															|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// Base class for everything attached to a GameObject.
//
class Component : public Object {
public: // Constructors & Destructors
	inline Component() = default;

	virtual ~Component() = default;

public: // Properties
	GameObject* gameObject = nullptr;	// The game object this component is attached to. A component is always attached to a game object.
	Transform* transform = nullptr;		// The Transform attached to this GameObject.
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | BEHAVIOUR			 															|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// Behaviours are Components that can be enabled or disabled.
//
class Behaviour : public Component {
public: // Constructors & Destructors
	inline Behaviour()
		: Component() {}

public: // Functions
	// 
	// Whether a GameObject and its associated Behaviour is active and enabled.
	// 
	// A GameObject can be active or inactive. Similarly, a Behaviour can be enabled or disabled. 
	// 
	// @return
	//		If a GameObject is active and has an enabled behaviour then isActiveAndEnabled will return true. 
	//		Otherwise return false.
	//
	[[nodiscard]] inline bool IsActiveAndEnabled() const noexcept;

public: // Properties
	bool enable = true;	// Enabled Behaviours are Updated, disabled Behaviours are not.
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | MONO BEHAVIOUR			 														|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// 
//
class MonoBehaviour : public Behaviour {

	//
	// Polymorphism might slow down the application.
	// Might need refactor in future development.
	//

public: // Functions
	virtual void Start() = 0;

	virtual void Update() = 0;
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | TRANSFORM			 															|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// Position, rotation and scale of an object.
// This class also contains information about the hierarchy position of 
// the attached GameObject.
//
class Transform : public Component {
public: // Constructors & Destructors
	// 
	// Create a new Transform with the given position, rotation and scale.
	//
	inline Transform(Vector2 _position, float _rotation, Vector2 _scale) 
		: m_localPosition(_position), m_localRotation(_rotation), m_localScale(_scale) {
		m_isRoot = m_parent == nullptr;

		RecalculateWorldTransform();
	}

	//
	// Create a new Transform identical to parent transform.
	//
	inline Transform()
		: Transform(Vector2::zero, 0, Vector2::one) {}

	inline ~Transform() = default;

public:	// Functions
	//
	// Unparents the given child transform.
	//
	inline void DetachChild(Transform* _child) noexcept {
		auto iter = std::find(m_children.begin(), m_children.end(), _child);
		if (iter != m_children.end())
			m_children.erase(iter);
	}

	//
	// Unparents all children.
	//
	inline void DetachChildren() noexcept {
		for (auto child : m_children) {
			child->SetParent(m_parent);
		}
		m_children.clear();
	}

	//
	// Is this transform a child of _parent transform?
	// 
	// @return
	//		A boolean value that indicates whether the transform is a child of a given transform. 
	//		True if this transform is a child, deep child (child of a child) or identical to this transform, 
	//		otherwise false.
	//
	[[nodiscard]] inline bool IsChildOf(Transform* _parent) const noexcept {
		if (m_parent)
			return m_parent->IsChildOf(_parent);
		else
			return this == _parent;
	}

	//
	// Get a child transform of index _index.
	// 
	// @param _index:
	//		The sibling index of child to get.
	// 
	// @return
	//		A pointer to the child transform if found.
	//		If no child of index _index was found, returns nullptr instead.
	//
	[[nodiscard]] inline Transform* GetChild(size_t _index) noexcept {
		if (_index >= GetChildCount()) return nullptr;
		return m_children[_index];
	}

	//
	// Transforms direction from local space to world space.
	// 
	// This operation is not affected by scale or position of the transform. 
	// The returned vector has the same length as direction.
	// 
	// Transform.TransformPoint should be used instead if the vector represents a position rather than a direction.
	//
	[[nodiscard]] inline Vector2 TransformDirection(Vector2 _direction) const noexcept {
		return Matrix3x3::Rotate(m_rotation).MultiplyPoint(_direction);
	}

	//
	// Transforms multiple directions from local space to world space overwriting each original direction with the transformed version.
	// 
	// This operation is not affected by scale or position of the transform. 
	// The transformed vectors have the same length as the original versions.
	// 
	// Transform.TransformPoints should be used instead if the vectors represents positions rather than directions.
	// 
	// @param _directions [OUT]:
	//		The directions to be transformed, each is replaced by the transformed version.
	//
	inline void TransformDirections(std::vector<Vector2>& _directions) const noexcept {
		Matrix3x3 matrix = Matrix3x3::Rotate(m_rotation);
		for (auto& direct : _directions) {
			direct = matrix.MultiplyPoint(direct);
		}
	}

	//
	// Transforms multiple directions from local space to world space writing the transformed directions to a possibly different location.
	// 
	// This operation is not affected by scale or position of the transform. 
	// The transformed vectors have the same length as the original versions.
	// 
	// Transform.TransformPoints should be used instead if the vectors represents positions rather than directions.
	// 
	// @param _inDirections:
	//		The directions to be transformed.
	// @param _outDirections [OUT]:
	//		Receives the transformed directions.
	//
	inline void TransformDirections(const std::vector<Vector2>& _inDirections, std::vector<Vector2>& _outDirections) const noexcept {
		Matrix3x3 matrix = Matrix3x3::Rotate(m_rotation);
		_outDirections.clear();
		for (auto direct : _inDirections) {
			_outDirections.push_back(matrix.MultiplyPoint(direct));
		}
	}

	//
	// Transforms point from local space to world space. 
	// 
	// NOTE:
	// The returned position is affected by scale. 
	// 
	// Transform.TransformDirection should be used instead for handling with direction vectors.
	//
	[[nodiscard]] inline Vector2 TransformPoint(Vector2 _point) const noexcept {
		return m_localToWorldMatrix.MultiplyPoint(_point);
	}

	//
	// Transforms multiple points from local space to world space overwriting each original point with the transformed version.
	// 
	// NOTE:
	// The positions of the returned points are affected by scale. 
	// 
	// Transform.TransformDirections should be used instead for handling with direction vectors.
	// 
	// @param _points [OUT]:
	//		The positions of the points to be transformed, each is replaced by the transformed version.
	//
	inline void TransformPoints(std::vector<Vector2>& _points) const noexcept {
		for (auto& point : _points) {
			point = TransformPoint(point);
		}
	}

	//
	// Transforms multiple points from local space to world space writing the transformed points to a possibly different location.
	// 
	// NOTE:
	// The positions of the returned points are affected by scale. 
	// 
	// Transform.TransformDirections should be used instead for handling with direction vectors.
	// 
	// @param _inPoints:
	//		The positions of the points to be transformed.
	// @param _outPoints [OUT]:
	//		Receives the transformed positions of each point.
	//
	inline void TransformPoints(const std::vector<Vector2>& _inPoints, std::vector<Vector2>& _outPoints) const noexcept {
		_outPoints.clear();
		for (auto point : _inPoints) {
			_outPoints.push_back(TransformPoint(point));
		}
	}

	//
	// Transforms a direction from world space to local space.
	// 
	// This operation is not affected by scale or position of the transform. 
	// The transformed vector has the same length as the original.
	// 
	// Transform.InverseTransformPoint should be used instead if the vector represents a position rather than a direction.
	//
	[[nodiscard]] inline Vector2 InverseTransformDirection(Vector2 _direction) const noexcept {
		return Matrix3x3::Rotate(m_rotation).Inverse().MultiplyPoint(_direction);
	}

	//
	// Transforms multiple directions from world space to local space overwriting each original position with the transformed version.
	// 
	// This operation is not affected by scale or position of the transform. 
	// The transformed vector has the same length as the original.
	// 
	// Transform.InverseTransformPoints should be used instead if the vectors represents positions rather than directions.
	// 
	// @param _directions [OUT]:
	//		The directions to be transformed, each is replaced by the transformed version.
	//
	inline void InverseTransformDirections(std::vector<Vector2>& _directions) const noexcept {
		Matrix3x3 matrix = Matrix3x3::Rotate(m_rotation).Inverse();
		for (auto& direct : _directions) {
			direct = matrix.MultiplyPoint(direct);
		}
	}

	//
	// Transforms multiple directions from world space to local space overwriting each original position with the transformed version.
	// 
	// This operation is not affected by scale or position of the transform. 
	// The transformed vector has the same length as the original.
	// 
	// Transform.InverseTransformPoints should be used instead if the vectors represents positions rather than directions.
	// 
	// @param _inDirections:
	//		The directions to be transformed.
	// @param _outDirections [OUT]:
	//		Receives the transformed directions.
	//
	inline void InverseTransformDirections(const std::vector<Vector2>& _inDirections, std::vector<Vector2>& _outDirections) const noexcept {
		Matrix3x3 matrix = Matrix3x3::Rotate(m_rotation).Inverse();
		_outDirections.clear();
		for (auto direct : _inDirections) {
			_outDirections.push_back(matrix.MultiplyPoint(direct));
		}
	}

	//
	// Transforms point from world space to local space. 
	// 
	// NOTE:
	// The positions of the returned positions are affected by scale. 
	// 
	// Transform.InverseTransformDirection should be used instead for handling with direction vectors.
	//
	[[nodiscard]] inline Vector2 InverseTransformPoint(Vector2 _point) const noexcept {
		return m_worldToLocalMatrix.MultiplyPoint(_point);
	}

	//
	// Transforms multiple positions from world space to local space overwriting each original position with the transformed version.
	// 
	// NOTE:
	// The positions of the returned positions are affected by scale. 
	// 
	// Transform.InverseTransformDirection should be used instead for handling with direction vectors.
	// 
	// @param _points:
	//		The positions of the points to be transformed, each is replaced by the transformed version.
	//
	inline void InverseTransformPoints(std::vector<Vector2>& _points) const noexcept {
		for (auto& point : _points) {
			point = InverseTransformPoint(point);
		}
	}

	//
	// Transforms multiple positions from world space to local space overwriting each original position with the transformed version.
	// 
	// NOTE:
	// The positions of the returned positions are affected by scale. 
	// 
	// Transform.InverseTransformDirection should be used instead for handling with direction vectors.
	// 
	// @param _intPoints:
	//		The positions of the points to be transformed.
	// @param _outPoints:
	//		Receives the transformed directions.
	//
	inline void InverseTransformPoints(const std::vector<Vector2>& _intPoints, std::vector<Vector2>& _outPoints) const noexcept {
		_outPoints.clear();
		for (auto& point : _intPoints) {
			_outPoints.push_back(InverseTransformPoint(point));
		}
	}

	//
	// Translate the local position by _move amount.
	// 
	// @param _move:
	//		The translate amount in world unit.
	//
	inline void TranslateLocal(Vector2 _move) noexcept {
		SetLocalPosition(m_localPosition + _move);
	}

	//
	// Translate the world position by _move amount.
	// 
	// @param _move:
	//		The translate amount in world unit.
	//
	inline void Translate(Vector2 _move) noexcept {
		SetPosition(m_position + _move);
	}

	//
	// Rotate the transform by _rotation amount.
	// 
	// @param _rotation:
	//		The rotation amount in degree.
	//
	inline void Rotate(float _rotation) noexcept {
		SetRotation(m_rotation + _rotation);
	}

	//
	// Multiple localScale of the transform by _scale amount.
	// 
	// @param _scale:
	//		The scaling amount.
	//
	inline void Scale(Vector2 _scale) noexcept {
		SetLossyScale(m_lossyScale.Scale(_scale));
	}

	//
	// Add _scale amount to the transform.
	// 
	// NOTE: 
	// This function adds a flat amount of scaling to the transform local scale.
	// To scale using multiplication, use Transform.Scale() instead.
	// 
	// @param _scale:
	//		The amount of scaling to add to the transform.
	//
	inline void AddScale(Vector2 _scale) noexcept {
		SetLossyScale(m_lossyScale + _scale);
	}

private:
	inline void RecalculateWorldPosition() noexcept {
		if (m_isRoot)
			m_position = m_localPosition;
		else
			m_position = m_parent->TransformPoint(m_localPosition);
	}

	inline void RecalculateLocalPosition() noexcept {
		if (m_isRoot)
			m_localPosition = m_position;
		else
			m_localPosition = m_parent->InverseTransformPoint(m_position);
	}

	inline void RecalculateWorldRotation() noexcept {
		if (m_isRoot)
			m_rotation = m_localRotation;
		else
			m_rotation = m_parent->m_rotation + m_localRotation;
	}

	inline void RecalculateLocalRotation() noexcept {
		if (m_isRoot)
			m_localRotation = m_rotation;
		else
			m_localRotation = m_rotation - m_parent->m_rotation;
	}

	inline void RecalculateLossyScale() noexcept {
		if (m_isRoot)
			m_lossyScale = m_localScale;
		else
			m_lossyScale = Vector2::Scale(m_parent->m_lossyScale, m_localScale);
	}

	inline void RecalculateLocalScale() noexcept {
		if (m_isRoot)
			m_localScale = m_lossyScale;
		else
			m_localScale = Vector2{
				m_lossyScale.x / m_parent->m_lossyScale.x,
				m_lossyScale.y / m_parent->m_lossyScale.y,
			};
	}

	inline void RecalculateWorldTransform() noexcept {
		RecalculateWorldPosition();
		RecalculateWorldRotation();
		RecalculateLossyScale();

		RecalculateMatrices();

		RecalculateChildTransforms();
	}

	inline void RecalculateLocalTransform() noexcept {
		RecalculateLocalPosition();
		RecalculateLocalRotation();
		RecalculateLocalScale();

		RecalculateChildTransforms();
	}

	inline void RecalculateChildTransforms() noexcept {
		for (auto child : m_children) {
			child->RecalculateWorldTransform();
		}
	}

	inline void RecalculateMatrices() noexcept {
		m_localToWorldMatrix.SetTRS(m_position, m_rotation, m_lossyScale);
		m_worldToLocalMatrix = m_localToWorldMatrix.Inverse();
	}

	inline void OnTransformChanged() {
		RecalculateMatrices();
		RecalculateChildTransforms();
	}

public: // Accessors
	// Get the local transformed Vector2.up of this transform.
	[[nodiscard]] inline Vector2 UpDirection() const {
		return TransformDirection(Vector2::up);
	}

	// Get the local transformed Vector2.right of this transform.
	[[nodiscard]] inline Vector2 RightDirection() const {
		return TransformDirection(Vector2::right);
	}

	// Get the number of children the parent Transform has.
	[[nodiscard]] inline size_t GetChildCount() const {
		return m_children.size();
	}

	// Get a pointer to the topmost transform in the hierarchy.
	[[nodiscard]] inline Transform* GetRoot() {
		return m_parent ? m_parent->GetParent() : this;
	}
	
	// Is this transform a root transform.
	// A root transform is a transform with no parent transform.
	[[nodiscard]] inline bool IsRoot() {
		return m_isRoot;
	}

	// Get the world space position of the Transform.
	[[nodiscard]] inline Vector2 GetPosition() const {
		return m_position;
	}

	// Get the position of the transform relative to the parent transform.
	[[nodiscard]] inline Vector2 GetLocalPosition() const {
		return m_localPosition;
	}

	// Get the rotation of the transform in world space.
	[[nodiscard]] inline float GetRotation() const {
		return m_rotation;
	}

	// Get the rotation of the transform relative to the parent transform.
	[[nodiscard]] inline float GetLocalRotation() const {
		return m_localRotation;
	}

	// Get the global scale of the transform.
	[[nodiscard]] inline Vector2 GetLossyScale() const {
		return m_lossyScale;
	}

	// Get the scale of the transform relative to the parent transform.
	[[nodiscard]] inline Vector2 GetLocalScale() const {
		return m_localScale;
	}

	// Get a pointer to the parent of the transform.
	// If this is a root transform, this returns a nullptr.
	[[nodiscard]] inline Transform* GetParent() const {
		return m_parent;
	}

	// Get the matrix that transforms a point from local space into world space.
	[[nodiscard]] inline Matrix3x3 LocalToWorldMatrix() const {
		return m_localToWorldMatrix;
	}

	// Get the matrix that transforms a point from world space into local space.
	[[nodiscard]] inline Matrix3x3 WorldToLocalMatrix() const {
		return m_worldToLocalMatrix;
	}

public: // Mutators
	// Set the world space position of the Transform.
	inline void SetPosition(Vector2 _position) {
		m_position = _position;
		RecalculateLocalPosition();

		OnTransformChanged();
	}

	// Set the position of the transform relative to the parent transform.
	inline void SetLocalPosition(Vector2 _localPosition) {
		m_localPosition = _localPosition;
		RecalculateWorldPosition();

		OnTransformChanged();
	}

	// Set the rotation of the transform in world space.
	inline void SetRotation(float _rotation) noexcept {
		m_rotation = _rotation;
		RecalculateLocalRotation();

		OnTransformChanged();
	}

	// Set the rotation of the transform relative to the parent transform.
	inline void SetLocalRotation(float _localRotation) noexcept {
		m_localRotation = _localRotation;
		RecalculateWorldRotation();

		OnTransformChanged();
	}

	// Set the global scale of the transform.
	inline void SetLossyScale(Vector2 _lossyScale) noexcept {
		m_lossyScale = _lossyScale;
		RecalculateLocalScale();

		OnTransformChanged();
	}

	// Set the scale of the transform relative to the parent transform.
	inline void SetLocalScale(Vector2 _localScale) noexcept {
		m_localScale = _localScale;
		RecalculateLossyScale();

		OnTransformChanged();
	}

	// Set the parent transform of this transform.
	//
	// @param _worldPositionStay:
	//		If true, the parent-relative position, scale and rotation are modified such that 
	//		the object keeps the same world space position, rotation and scale as before.
	inline void SetParent(Transform* _parent, bool _worldPositionStay = true) noexcept {
		if (!m_isRoot)
			m_parent->DetachChild(this);

		m_parent = _parent;
		m_isRoot = m_parent == nullptr;
		if (!m_isRoot)
			m_parent->m_children.push_back(this);

		if (_worldPositionStay)
			RecalculateLocalTransform();
		else
			RecalculateWorldTransform();
	}
	 
protected: // Properties
	Vector2 m_localPosition = Vector2::zero;
	float m_localRotation = 0;
	Vector2 m_localScale = Vector2::one;

	Vector2 m_position = Vector2::zero;
	float m_rotation = 0;
	Vector2 m_lossyScale = Vector2::one;

	Matrix3x3 m_localToWorldMatrix;
	Matrix3x3 m_worldToLocalMatrix;

	bool m_isRoot;
	Transform* m_parent = nullptr;
	std::vector<Transform*> m_children = {};
};

// +--------------------------------------------------------------------------------+
// |																				|
// | GAME OBJECT																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Base class for all entities in a Scene.
// 
// This is an abstract wrapper for the Entity in the underlying ECS.
//
class GameObject final : public Object {
private:
	using component_id_type = std::size_t;

public: // Constructors & Destructors
	//
	// Create a new GameObject and attach it to 
	// 
	// @param _scene:
	//		The Scene this GameObject is attached to.
	// @param _tag:
	//		The name value of this GameObject.
	//
	inline GameObject(Scene* _scene, const std::string& _name)
		: GameObject() {
		scene = _scene;
		name = _name;
	}

	//
	// Create a new empty GameObject with a Transform component attached.
	//
	inline GameObject() {
		// Every GameObject need to have a Transform component attached to it.
		transform = AddComponent<Transform>();
		transform->transform = transform;	// The transform property of a Transform component will always point to itself.
	}

	inline ~GameObject() {
		for (auto component : m_components) {
			delete component.second;
		}
	}

public: // Functions
	//
	// Add a new Component to this GameObject.
	// 
	// @tparam Type:
	//		A valid Component type to add.
	//		For a Type to be valid, it have to be derived from lmk::Component class.
	// 
	// @param _component:
	//		If NULL, the GameObject will create a new default Component of Type.
	//		If not NULL, the value of _component will be copied to the new Component.
	// 
	// @return
	//		As std::shared_ptr to the created Component.
	//		If failed to add component, return nullptr instead.
	//
	template <typename Type>
	inline Type* AddComponent() {
		LMK_CORE_ASSERT(IsComponent<Type>(), 
			"lmk::GameObject: AddComponent() - Invalid component type. Type must derived from lmk::Component.");

		if (HasComponent<Type>()) return nullptr;	// If component already added.

		Type* component = new Type();
		component->gameObject = this;
		component->transform = transform;

		m_components[GetComponentID<Type>()] = static_cast<Component*>(component);

		return component;
	}

	//
	// Get a Component of Type from this GameObject.
	// 
	// @tparam Type:
	//		A valid Component type to get.
	//		For a Type to be valid, it have to be derived from lmk::Component class.
	// 
	// @return
	//		A pointer to the component of Type if found.
	//		If not, return nullptr instead.
	//
	template <typename Type>
	[[nodiscard]] inline Type* GetComponent() {
		LMK_CORE_ASSERT(IsComponent<Type>(),
			"lmk::GameObject: GetComponent() - Invalid component type. Type must derived from lmk::Component.");

		if (!HasComponent<Type>()) return nullptr;

		return static_cast<Type*>(m_components[GetComponentID<Type>()]);
	}

	//
	// Remove a Component of Type from this GameObject.
	// 
	// @tparam Type:
	//		A valid Component type to remove.
	//		For a Type to be valid, it have to be derived from lmk::Component class.
	//
	template <typename Type>
	inline void RemoveComponent() {
		LMK_CORE_ASSERT(IsComponent<Type>(),
			"lmk::GameObject: RemoveComponent() - Invalid component type. Type must derived from lmk::Component.");

		if (!HasComponent<Type>()) return;

		auto id = GetComponentID<Type>();
		delete m_components[id];
		m_components.erase(id);
	}

	//
	// Does this GameObject has a Component of Type attached.
	// 
	// @tparam Type:
	//		A valid Component type to check for.
	//		For a Type to be valid, it have to be derived from lmk::Component class.
	//
	template <typename Type>
	bool HasComponent() {
		return m_components.find(GetComponentID<Type>()) != m_components.end();
	}

private:
	//
	// Is the given Type a valid Component type.
	//
	template <typename Type>
	[[nodiscard]] inline static bool IsComponent() {
		return std::is_base_of<Component, Type>::value;
	}

	template <typename Type>
	[[nodiscard]] constexpr component_id_type GetComponentID() {
		return component_id_type(typeid(Type).hash_code());
	}

public: // Properties
	bool selfActive = true;	// Is this GameObject active?

	std::string name;	// Name of the GameObject.

	Transform* transform;	// The Transform attached to this GameObject.
	Scene* scene;			// Scene that the GameObject is part of.

private:
	// Since we do not have an proper DOD ECS system implemented, we will assign ownership
	// of the components to the GameObject itself.
	std::unordered_map<component_id_type, Component*> m_components;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | GAME SCENE																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Run-time data structure presenting a hierarchy of GameObjects.
// 
// This class also acts as a registry of Components for internal systems to reference from.
// All creation of Component from a GameObject will be registered by a Scene.
//
class Scene {

	//
	// NOTE:
	// Normally, this class should only be initialized from a custom *.scene file through SceneManager.
	// A Scene Editor would be used for writing a Scene object to a *.scene file.
	// 
	// But since we have not yet have either a custom *.scene file format or a Scene Editor,
	// we will have to manually create the Scene object with the GameObject hierarchy in the source code.
	// 

public: // Constructors & Destructors
	//
	// Create a new empty Scene with given name.
	//
	inline Scene(std::string _name)
		: name(_name), m_sceneID(CreateID()) {}

	//
	// Create a new default empty Scene.
	//
	inline Scene()
		: Scene("Default Scene") {}

public: // Operator overloads
	inline bool operator==(const Scene& _other) {
		return m_sceneID == _other.m_sceneID;
	}

	inline bool operator!=(const Scene& _other) {
		return m_sceneID != _other.m_sceneID;
	}

public: // Functions
	//
	// Create a new GameObject an attach it to the current Scene.
	//
	inline GameObject& CreateGameObject(const std::string& _name = std::string()) {
		GameObject* newObject = new GameObject{ this, _name };
		m_objects.push_back(newObject);
		return *newObject;
	}

	//
	// Create a new GameObject and add it to the scene.
	// 
	// @param _original:
	//		An existing GameObject to make a copy of.
	// 
	// @return
	//		A reference to the instantiated clone of _original.
	//
	inline GameObject& Instantiate(const GameObject& _original) {
		GameObject* newObject = new GameObject(_original);
		m_objects.push_back(newObject);
		return *newObject;
	}

	// -------------------- TEMPORARY IMPLEMENTATION -------------------- //
	// 
	// At the point of the implementation of these functions, we do not
	// have an ECS system working properly.
	// 
	// Therefore we had to submit components into the relevant list manually.
	


	// ------------------------------------------------------------------ //

private:
	//
	// Create a unique ID for this Scene.
	//
	inline static uint32_t CreateID() {
		static uint32_t counter = 0;
		return counter++;
	}

public: // Properties
	std::string name;	// The name of the Scene.

private:
	uint32_t m_sceneID;	// The ID of the scene.

	std::vector<GameObject*> m_objects;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | SCENE MANAGER																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Scene management Get run-time.
// 
// NOTE:
// SceneManager does not support multiple loaded Scene.
// Loading a new Scene will overwrite the old Scene, meaning all GameObjects in old Scene
// will be unloaded.
//
class SceneManager {
public: // Constructors & Destructors
#pragma region Singleton
	SceneManager(const SceneManager&) = delete;

private:
	inline SceneManager()
		: m_currentScene(new Scene()) {}

public:
#pragma endregion

public:
	//
	// Is _scene loaded?
	//
	[[nodiscard]] inline static bool IsLoaded(Scene* _scene) {
		return _scene == Instance.m_currentScene.get();
	}

	//
	// Load a new Scene.
	// 
	// This will set the current scene to _scene.
	//
	inline static void LoadScene(Scene* _scene) {
		Instance.m_currentScene = std::shared_ptr<Scene>(_scene);
	}

public: // Accessors
	// Get the currently loaded Scene.
	inline static std::shared_ptr<Scene> GetCurrentScene() {
		return Instance.m_currentScene;
	}

private: // Static Properties
	static SceneManager Instance;

private: // Properties
	// Right now SceneManager will only allow for only one instance of any Scene to be loaded Get any time.
	// This might change in future developments.
	std::shared_ptr<Scene> m_currentScene;
};

SceneManager SceneManager::Instance;



// 0========================================================================================================0
// |																										|
// | SEPERATE IMPLEMENTATION																				|
// |																										|
// 0========================================================================================================0

// +--------------------------------------------------------------------------------+
// |																				|
// | BEHAVIOUR																		|
// |																				|
// +--------------------------------------------------------------------------------+

[[nodiscard]] inline bool Behaviour::IsActiveAndEnabled() const noexcept {
	return enable && gameObject->selfActive;
}

LMK_END

#endif // !_LMK_GAMEOBJ_H_