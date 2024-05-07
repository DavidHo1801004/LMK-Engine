#ifndef SUPERMARIOCLONE_H_
#define SUPERMARIOCLONE_H_

#define LMK_INCLUDE_ALL
#include "LMK/LMK.h"

using namespace lmk;

#pragma warning(disable : 4244)
class SuperMarioClone : public LMKEngine {
private: // Properties
	Vector2Int mousePos;

	bool updated = false;
	bool overlapping = false;

	Vector2 offset = Vector2::one() * 200;
	float rotation = 0;
	Vector2 scale = Vector2::one();

	PolygonCollider2D playerCollider;
	Vector2 direction;

	std::vector<BoxCollider2D> staticColliders;

	const float movespeed = 100;
	const float rotationSpeed = 90;
	const Vector2 scaleSpeed = Vector2(1.0f, 1.5f);

public: // Functions
	inline void OnUserStart() override {
		auto CreatePolygon = [&](uint8_t _corners, float _radius) {
			auto rotationIter = 360 / _corners;

			std::vector<Vector2> vertices = { Vector2(0, _radius) };
			for (uint8_t i = 1; i < _corners; i++) {
				vertices.push_back(Vector2(
					-1 * _radius * std::sinf(LMK_DtoR(rotationIter * i)),
					_radius * std::cosf(LMK_DtoR(rotationIter * i)))
				);
			}
			return vertices;
		};

		playerCollider = PolygonCollider2D(
			CreatePolygon(5, 30)
		);

		playerCollider.Transform(offset, rotation, scale);

		for (int i = 0; i < 10; i++) {
			staticColliders.push_back(BoxCollider2D(Vector2(30, 30), 0));
			staticColliders[i].Transform(
				Vector2(Random::Range<float>(30, m_screenSize.x - 30), Random::Range<float>(30, m_screenSize.y - 30)),
				Random::Range<float>(0, 359),
				Vector2(Random::Range<float>(0.5f, 3.0f), Random::Range<float>(0.5f, 3.0f)));
		}
	}

	inline void OnUserUpdate() override {
		// Transform player's collider.
		if (Input::GetKey(KeyCode::D)) {
			direction.x = 1;
		}
		else if (Input::GetKey(KeyCode::A)) {
			direction.x = -1;
		}
		else {
			direction.x = 0;
		}

		if (Input::GetKey(KeyCode::W)) {
			direction.y = -1;
		}
		else if (Input::GetKey(KeyCode::S)) {
			direction.y = +1;
		}
		else {
			direction.y = 0;
		}

		offset += direction.normalized() * movespeed * Time::DeltaTime();

		if (Input::GetKey(KeyCode::E)) {
			rotation += rotationSpeed * Time::DeltaTime();
		}
		else if (Input::GetKey(KeyCode::Q)) {
			rotation -= rotationSpeed * Time::DeltaTime();
		}

		if (Input::GetKey(KeyCode::R)) {
			scale += scaleSpeed * Time::DeltaTime();
		}
		else if (Input::GetKey(KeyCode::F)) {
			scale -= scaleSpeed * Time::DeltaTime();
		}

		playerCollider.Transform(offset, rotation, scale);

		for (auto staticCollider : staticColliders) {
			if (playerCollider.OverlapWith(staticCollider)) {
				overlapping = true;
				break;
			}
			else {
				overlapping = false;
			}
		}
	}

	inline void OnDrawGizmos() override {
		if (overlapping) {
			m_gizmo->SetColor(Color::blue());
		} 
		else {
			m_gizmo->SetColor(Color::white());
		}

		m_gizmo->DrawPolygon(playerCollider.vertices());
		m_gizmo->DrawLine(offset, playerCollider.vertex(0));

		if (scale.x < 0 || scale.y < 0) {
			m_gizmo->SetColor(Color::red());
			m_gizmo->DrawLine(playerCollider.vertex(0), playerCollider.vertex(2));
			m_gizmo->DrawLine(playerCollider.vertex(1), playerCollider.vertex(3));
		}

		m_gizmo->SetColor(Color::yellow());

		for (auto staticCollider : staticColliders) {
			m_gizmo->DrawPolygon(staticCollider.vertices());
		}
	}
};
#pragma warning(default : 4244)

#endif // !SUPERMARIOCLONE_H_