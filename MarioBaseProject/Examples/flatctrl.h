#pragma once
#include "LMK.h"

class FlatControlBehaviour : public lmk::MonoBehaviour {
protected:
	inline void OnUpdate() override {
		moveDir = Vector2::zero;
		if (Input::GetKey(KeyCode::W)) {
			moveDir.y += 1;
		}
		if (Input::GetKey(KeyCode::S)) {
			moveDir.y -= 1;
		}
		if (Input::GetKey(KeyCode::A)) {
			moveDir.x -= 1;
		}
		if (Input::GetKey(KeyCode::D)) {
			moveDir.x += 1;
		}

		rotateDir = 0;
		if (Input::GetKey(KeyCode::Q)) {
			rotateDir += 1;
		}
		if (Input::GetKey(KeyCode::E)) {
			rotateDir -= 1;
		}

		if (!targetBody) {
			transform->Translate(moveDir * movementSpeed * Time::GetDeltaTime());
			transform->Rotate(rotateDir * rotateSpeed * Time::GetDeltaTime());
		}
		else {
			targetBody->linearVelocity = moveDir * movementSpeed;
			targetBody->angularVelocity = rotateDir * rotateSpeed;
		}
	}

public:
	Rigidbody2D* targetBody;

	float movementSpeed = 5.0f; // Units
	float rotateSpeed = 90.0f; // Degrees

private:
	Vector2 moveDir;
	float rotateDir;
};