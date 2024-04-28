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

	PhysicsShape2D phyShape;
	BaseCollider2D playerCollider;
	BoxCollider2D staticCollider = BoxCollider2D(Vector2(30, 30), 0);

	const float speed = 400;

public: // Functions
	inline void OnUserStart() override {
		phyShape.vert.clear();
		phyShape.vert.push_back(Vector2(-30, 30));
		phyShape.vert.push_back(Vector2(30, 30));
		phyShape.vert.push_back(Vector2(30, -30));
		phyShape.vert.push_back(Vector2(-30, -30));

		playerCollider = BaseCollider2D(phyShape);

		playerCollider.Transform(offset, rotation, scale);
		staticCollider.Transform(Vector2::one() * 400, 0, Vector2::one());
	}

	inline void OnUserHandleEvents(const SDL_Event& _event) override {
		switch (_event.type) {
		case SDL_MOUSEMOTION:
			mousePos.x = _event.motion.x;
			mousePos.y = _event.motion.y;
			break;

		case SDL_KEYDOWN:
			switch (_event.key.keysym.sym) {
			case SDLK_e:
				updated = true;
				rotation += 360 * Time::DeltaTime();
				break;

			case SDLK_q:
				updated = true;
				rotation -= 360 * Time::DeltaTime();
				break;

			case SDLK_w:
				updated = true;
				offset.y -= speed * Time::DeltaTime();
				break;

			case SDLK_s:
				updated = true;
				offset.y += speed * Time::DeltaTime();
				break;

			case SDLK_a:
				updated = true;
				offset.x -= speed * Time::DeltaTime();
				break;

			case SDLK_d:
				updated = true;
				offset.x += speed * Time::DeltaTime();
				break;

			case SDLK_f:
				updated = true;
				scale -= Vector2(0.05f, 0.1f);
				break;

			case SDLK_r:
				updated = true;
				scale += Vector2(0.05f, 0.1f);
				break;
			}
			break;
		}
	}

	inline virtual void OnUserUpdate() override {
		if (updated) {
			playerCollider.Transform(offset, rotation, scale);
		}

		if (playerCollider.OverlapWith(staticCollider)) {
			overlapping = true;
		}
		else {
			overlapping = false;
		}
	}

	inline virtual void OnUserRender() override {
		if (overlapping) {
			m_gizmo->SetColor(Color::blue());
		} 
		else {
			m_gizmo->SetColor(Color::white());
		}

		m_gizmo->DrawPolygon(playerCollider.tvert);

		if (scale.x < 0 || scale.y < 0) {
			m_gizmo->SetColor(Color::red());
			m_gizmo->DrawLine(playerCollider.tvert[0], playerCollider.tvert[2]);
			m_gizmo->DrawLine(playerCollider.tvert[1], playerCollider.tvert[3]);
		}

		m_gizmo->SetColor(Color::yellow());
		m_gizmo->DrawPolygon(staticCollider.tvert);
	}
};
#pragma warning(default : 4244)

#endif // !SUPERMARIOCLONE_H_