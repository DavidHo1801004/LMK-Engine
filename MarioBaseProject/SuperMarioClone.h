#ifndef SUPERMARIOCLONE_H_
#define SUPERMARIOCLONE_H_

#define LMK_INCLUDE_ALL
#include "LMK/LMK.h"

class SuperMarioClone : public lmk::lmkEngine {
public: // Functions
	inline void OnUserStart() override {
		m_rotation = 0;
		m_moveRect = lmk::Rect(lmk::Vector2::zero(), lmk::Vector2::one() * 100);
		m_staticRect = lmk::Rect(m_screenSize / 2, lmk::Vector2::one() * 200);

		m_overlapping = false;
	}

	inline void OnUserHandleEvents(const SDL_Event& _event) override {
		switch (_event.type) {
		case SDL_MOUSEMOTION:
			m_mousePos.x = _event.motion.x;
			m_mousePos.y = _event.motion.y;
			break;

		case SDL_KEYDOWN:
			switch (_event.key.keysym.sym)
			{
			case SDLK_e:
				m_rotation -= 45 * lmk::Time::DeltaTime();
				break;

			case SDLK_q:
				m_rotation += 45 * lmk::Time::DeltaTime();
				break;

			case SDLK_w:
				m_moveRect.Scale(1 + 10 * lmk::Time::DeltaTime());
				break;

			case SDLK_s:
				m_moveRect.Scale(1 - 10 * lmk::Time::DeltaTime());
				break;

			case SDLK_a:
				m_staticRect.Offset(-10, 0);
				break;

			case SDLK_d:
				m_staticRect.Offset(10, 0);
				break;
			}
			break;
		}
	}

	inline virtual void OnUserUpdate() override {
		m_overlapping = m_staticRect.Overlaps(m_moveRect);
		m_moveRect.setPosition(m_mousePos);
	}

	inline virtual void OnUserRender() override {
		m_gizmo->setColor(lmk::Color::yellow());
		m_gizmo->DrawRect(m_staticRect);

		if (m_overlapping) {
			m_gizmo->setColor(lmk::Color::red());
		}
		else {
			m_gizmo->setColor(lmk::Color::white());
		}

		m_gizmo->DrawRect(m_moveRect, m_rotation);
	}

private: // Properties
	lmk::Vector2Int m_mousePos;

	bool m_overlapping;
	float m_rotation;
	lmk::Rect m_moveRect;
	lmk::Rect m_staticRect;
};

#endif // !SUPERMARIOCLONE_H_