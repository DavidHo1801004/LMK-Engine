#pragma once
#ifndef LMK_GAMEOBJ_H_
#define LMK_GAMEOBJ_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"

LMK_BEGIN
// +--------------------------------------------------------------------------------+
// |					 															|
// | TRANSFORM			 															|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// Position, rotation and scale of an object.
//
class Transform {
public: // Constructors & Destructors
	inline Transform() {

	}

public:	// Functions
	inline void SetAsFirstSibling() {}
	inline void SetAsLastSibling() {}

	inline void DetachChildren() {}

	_NODISCARD inline bool IsChildOf(const Transform& _parent) const {}

	_NODISCARD inline Transform* Find(std::string _name) const {}

	_NODISCARD inline Transform* GetChild(int _index) const {}

	_NODISCARD inline Vector2 TransformDirection(const Vector2& _direction) const {}
	_NODISCARD inline std::vector<Vector2> TransformDirections(std::vector<Vector2> _directions) const {}

	_NODISCARD inline Vector2 TransformPoint(const Vector2& _point) const {}
	_NODISCARD inline std::vector<Vector2> TransformPoints(std::vector<Vector2> _points) const {}

	_NODISCARD inline Vector2 InverseTransformDirection(const Vector2& _direction) const {}
	_NODISCARD inline std::vector<Vector2> InverseTransformDirections(std::vector<Vector2> _directions) const {}

	_NODISCARD inline Vector2 InverseTransformPoint(const Vector2& _point) const {}
	_NODISCARD inline std::vector<Vector2> InverseTransformPoints(std::vector<Vector2> _points) const {}

	_NODISCARD inline void Rotate(float _angle, bool _worldSpace = false) {}

	_NODISCARD inline void RotateAround(const Vector2& _point, float _angle) {}

	_NODISCARD inline void Translate(const Vector2& _translation) {}
	_NODISCARD inline void Translate(const Vector2& _translation, bool _worldSpace = false) {}
	_NODISCARD inline void Translate(const Vector2& _translation, const Transform& _relativeTo) {}

public: // Accessors & Mutators
	_NODISCARD inline uint8_t childCount() const {}

	_NODISCARD inline Vector2 up() const {}

	_NODISCARD inline Vector2 right() const {}

	_NODISCARD inline Transform* root() {}

	_NODISCARD inline Vector2 position() const {}
	inline void setPosition(Vector2 _newVal) {}

	_NODISCARD inline Vector2 getLocalPosition() const {}
	inline void setLocalPosition(Vector2 _newVal) {}

	_NODISCARD inline float getRotation() const {}
	inline void setRotation(float _newVal) {}

	_NODISCARD inline float getLocalRotation() const {}
	inline void setLocalRotation(float _newVal) {}

	_NODISCARD inline Vector2 getLossyScale() {}

	_NODISCARD inline Vector2 getLocalScale() const {}
	inline void setLocalScale(Vector2 _newVal) {}

	_NODISCARD inline int getSiblingIndex() const {}
	inline void setSiblingIndex(int _index) {}

	_NODISCARD inline Transform* getParent() const {}
	inline void setParent(Transform* _p) {}
	inline void setParent(Transform* _parent, bool _worldPositionStays) {}

protected: // Properties
	Vector2 localscale = Vector2::one();
	Vector2 localPosition = Vector2::zero();
	float localrotation = 0;

	bool isRoot;
	int siblingIndex;
	Transform* parent = nullptr;
	std::vector<Transform*> childs;
};

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
class RectTransform : public Transform {
public:

};

// +--------------------------------------------------------------------------------+
// |																				|
// | GAME OBJECT																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// Base class for all entities in a Scene.
//
class GameObject {
public:
	GameObject() {}

public:
	RectTransform* transform = nullptr;
};
LMK_END

#endif // !LMK_GAMEOBJ_H_