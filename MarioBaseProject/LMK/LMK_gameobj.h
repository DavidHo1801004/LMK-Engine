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

using component_id_type = std::type_index;

// +--------------------------------------------------------------------------------+
// |					 															|
// | COMPONENT			 															|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// 
//
class Component : public Object {
public: // Constructors & Destructors
	inline Component() = default;

	virtual ~Component() = default;

public: // Functions
	//
	// OnStart is called when the object is created.
	//
	virtual void OnStart() {};

	//
	// OnUpdate is called every frame.
	//
	virtual void OnUpdate() {};

	//
	// OnDestroy is called when 
	//
	virtual void OnDestroy() {};

public: // Properties
	GameObject* gameObject = nullptr;
	Transform* transform = nullptr;

	std::string tag;
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
	inline Transform(Vector2 _position, float _rotation, Vector2 _scale) 
		: m_localPosition(_position), m_localRotation(_rotation), m_localScale(_scale) {}

	inline Transform() = default;

public:	// Functions
	inline void SetAsFirstSibling() {

	}

	inline void SetAsLastSibling() {

	}

	inline void DetachChildren() {

	}

	_NODISCARD inline bool IsChildOf(const Transform& _parent) const {

	}

	_NODISCARD inline Transform* Find(std::string _name) const {

	}

	_NODISCARD inline Transform* GetChild(int _index) const {

	}

	_NODISCARD inline Vector2 TransformDirection(Vector2 _direction) const {

	}

	_NODISCARD inline std::vector<Vector2> TransformDirections(std::vector<Vector2> _directions) const {

	}

	_NODISCARD inline Vector2 TransformPoint(Vector2 _point) const {

	}

	_NODISCARD inline std::vector<Vector2> TransformPoints(std::vector<Vector2> _points) const {

	}

	_NODISCARD inline Vector2 InverseTransformDirection(Vector2 _direction) const {

	}

	_NODISCARD inline std::vector<Vector2> InverseTransformDirections(std::vector<Vector2> _directions) const {

	}

	_NODISCARD inline Vector2 InverseTransformPoint(Vector2 _point) const {

	}

	_NODISCARD inline std::vector<Vector2> InverseTransformPoints(std::vector<Vector2> _points) const {

	}

	inline void Rotate(float _angle, bool _worldSpace = false) {

	}

	inline void RotateAround(Vector2 _point, float _angle) {

	}

private:
	inline void UpdateTransform() {

	}

public: // Accessors
	_NODISCARD inline uint8_t childCount() const {

	}

	_NODISCARD inline Vector2 up() const {

	}

	_NODISCARD inline Vector2 right() const {

	}

	_NODISCARD inline Transform* root() {

	}

	_NODISCARD inline Vector2 position() const {
		return m_localPosition + (m_parent ? m_parent->position() : Vector2::zero());
	}

	_NODISCARD inline Vector2 localPosition() const {
		return m_localPosition;
	}

	_NODISCARD inline float rotation() const {
		return m_localRotation + (m_parent ? m_parent->rotation() : 0);
	}

	_NODISCARD inline float localRotation() const {
		return m_localRotation;
	}

	_NODISCARD inline Vector2 lossyScale() {
		return m_localScale.Scale(m_parent ? m_parent->lossyScale() : Vector2::one());
	}

	_NODISCARD inline Vector2 localScale() const {

	}

	_NODISCARD inline int getSiblingIndex() const {

	}

	_NODISCARD inline Transform* parent() const {

	}

public: // Mutators
	inline void setPosition(Vector2 _newVal) {
		m_localPosition = _newVal - (m_parent ? m_parent->position() : Vector2::zero());
	}

	inline void setLocalPosition(Vector2 _newVal) {
		m_localPosition = _newVal;
	}

	inline void setRotation(float _newVal) {

	}

	inline void setLocalRotation(float _newVal) {
		m_localRotation = _newVal;
	}

	inline void setLossyScale(Vector2 _newVal) {

	}

	inline void setLocalScale(Vector2 _newVal) {
		m_localScale = _newVal;
	}

	inline void setSiblingIndex(int _index) {

	}

	inline void setParent(Transform* _parent, bool _worldPositionStays) {
		m_parent = std::shared_ptr<Transform>(_parent);
		if (_worldPositionStays) {

		}
	}

protected: // Properties
	Vector2 m_localScale = Vector2::one();
	Vector2 m_localPosition = Vector2::zero();
	float m_localRotation = 0;

	bool m_isRoot;
	int m_siblingIndex;
	std::shared_ptr<Transform> m_parent = nullptr;
	std::vector<std::shared_ptr<Transform>> m_children;
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
public: // Constructors & Destructors
	//
	// Create a new GameObject and attach it to 
	// 
	// @param _scene:
	//		The Scene this GameObject is attached to.
	// @param _tag:
	//		The tag value of this GameObject.
	//
	inline GameObject(Scene* _scene, const std::string& _tag)
		: scene(_scene), tag(_tag) {
		// Every GameObject need to have a Transform component attached to it.
		// We add this here to assure the transform property of the Transform component is valid.
		transform = new Transform();
		transform->gameObject = this;
		transform->transform = transform;
		m_components[component_id_type(typeid(Transform))] = std::shared_ptr<Transform>(transform);
	}

	inline GameObject() = default;

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
	std::shared_ptr<Type> AddComponent() {
		if (!IsComponent<Type>()) return nullptr;
		if (HasComponent<Type>()) return nullptr;

		// Create a new Component the component pointer to the correct Type.
		std::shared_ptr<Component> component = std::make_shared<Component>();
		component->gameObject = this;
		component->transform = transform;

		// Cast component pointer to the correct Type.
		auto castComponent = std::dynamic_pointer_cast<Type>(component);
		m_components[component_id_type(typeid(Type))] = castComponent;

		return castComponent;
	}

	//
	// Get a Component of Type from this GameObject.
	// 
	// @return
	//		
	//
	template <typename Type>
	std::shared_ptr<Type> GetComponent() {
		if (!HasComponent<Type>()) return nullptr;

		return std::dynamic_pointer_cast<Type>(m_components[component_id_type(typeid(Type))]);
	}

	//
	// Does this GameOBject has a Component of Type attached.
	// 
	// @tparam Type:
	//		The type of Component to check for.
	//
	template <typename Type>
	bool HasComponent() {
		return m_components.find(component_id_type(typeid(Type))) != m_components.end();
	}

private:
	//
	// Is the given Type a valid Component type.
	//
	template <typename Type>
	_NODISCARD inline bool IsComponent() {
		return std::is_base_of<Component, Type>::value;
	}

public: // Properties
	bool isActive = true;
	std::string tag;

	Transform* transform;
	Scene* scene;

private:
	std::unordered_map<component_id_type, std::shared_ptr<Component>> m_components;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | GAME SCENE																		|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Run-time data structure presenting a hierarchy of GameObjetcs.
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

private: // Typedef
	using object_id = uint32_t;

	friend class GameObject;

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
	inline GameObject CreateGameObject(const std::string& _name = std::string()) {
		return { this, _name };
	}

	//
	// Add a new Component to this GameObject.
	// 
	// @tparam Type:
	//		A valid Component type to add.
	//		For a Type to be valid, it have to be derived from lmk::Component class.
	// 
	// @param _component:
	//		A pointer to the Component to register.
	//
	template <typename Type>
	void RegisterComponent(std::shared_ptr<Type> _component) {
		auto componentID = component_id_type(typeid(Type));
	}

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

	// The registry of this Scene.
	// This is a map of all components in a Scene.
	std::unordered_map<component_id_type, std::unordered_map<object_id, std::shared_ptr<Component>>> m_registry;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | SCENE MANAGER																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Scene management at run-time.
// 
// NOTE:
// SceneManager does not support multiple loaded Scene.
// Loading a new Scene will overwrite the old Scene, meaning all GameObjects in old Scene
// will be unloaded.
//
class SceneManager {
public: // Constructors & Destructors
#pragma region Singleton
	inline SceneManager(const SceneManager&) = delete;

private:
	inline SceneManager()
		: m_currentScene(new Scene()) {}

public:
#pragma endregion

public:
	//
	// Is _scene loaded?
	//
	_NODISCARD inline static bool IsLoaded(Scene* _scene) {
		return std::shared_ptr<Scene>(_scene) == Instance.m_currentScene;
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
	inline static std::shared_ptr<Scene> currentScene() {
		return Instance.m_currentScene;
	}

private: // Properties
	static SceneManager Instance;

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
// | RECT TRANSFORM																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Position, size, anchor and pivot information for a rectangle.
// 
// It's used to store and manipulate the position, size, and anchoring of a rectangle 
// and supports various forms of scaling based on a parent RectTransform.
//
class RectTransform final : public Transform {
public:

};
LMK_END

#endif // !_LMK_GAMEOBJ_H_