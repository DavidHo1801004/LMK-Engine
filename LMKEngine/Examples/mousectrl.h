#pragma once
#include "LMK.h"

class MouseControlBehaviour : public lmk::MonoBehaviour {
protected:
	inline void OnStart() override {
		mainCamera = Camera2D::GetMainCamera();
	}

	inline void OnUpdate() override {
		if (Input::GetMouseButtonDown(MouseButton::Middle)) {
			transform->SetLocalPosition(Vector2::zero);
			transform->SetLocalRotation(0);
		}
		
		if (Input::GetMouseButton(MouseButton::Left)) {
			transform->SetLocalPosition(Vector2::MoveTowards(
				transform->GetLocalPosition(),
				GetWorldMousePos(),
				movementSpeed * Time::GetDeltaTime()
			));
		}

		if (Input::GetMouseButton(MouseButton::Right)) {
			float targetAngle = Vector2::SignedAngle(
				transform->RightDirection(), 
				(GetWorldMousePos() - transform->GetPosition().GetNormalized()));

			int rotateDir = 0;
			if (targetAngle > 1e-6f) {
				rotateDir = 1;
			}
			else if (targetAngle < -1e-6f) {
				rotateDir = -1;
			}

			transform->SetRotation(transform->GetRotation() + rotateDir * rotateSpeed * Time::GetDeltaTime());
		}
	}

private:
	inline Vector2 GetWorldMousePos() const {
		return mainCamera->ScreenToWorldPoint(Input::GetMousePosition());
	}

public:
	float movementSpeed = 5.0f;
	float rotateSpeed = 90.0f;

private:
	Camera2D* mainCamera;
};