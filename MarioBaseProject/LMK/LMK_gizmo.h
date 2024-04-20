#ifndef LMK_GISMO_H_
#define LMK_GISMO_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL
#include "LMK_coremdl.h"
#include "LMK_render.h"

LMK_BEGIN
class Gizmo {
public:
	inline Gizmo(SDL_Renderer* _renderer) : m_renderer(_renderer) {}

public:
	//
	//
	//
	inline void DrawLine(const Vector2& _from, const Vector2& _to) {
		SDL_RenderDrawLineF(m_renderer, _from.x, _from.y, _to.x, _to.y);
	}

	//
	// Draw a rectangle on screen.
	//
	inline void DrawRect(const Rect& _rect, float _angle = 0) {
		Vector2 points[4];

		Vector2 rotaPivot = _rect.getPosition() + Vector2::Scale(_rect.center, _rect.getSize());

		// We need to transform the point before render it to screen.
		points[0] = RenderUtil::RotatePointAround(_rect.getMin(), rotaPivot, _angle);
		points[1] = RenderUtil::RotatePointAround(Vector2(_rect.getXMax(), _rect.getYMin()), rotaPivot, _angle);
		points[2] = RenderUtil::RotatePointAround(_rect.getMax(), rotaPivot, _angle);
		points[3] = RenderUtil::RotatePointAround(Vector2(_rect.getXMin(), _rect.getYMax()), rotaPivot, _angle);

		DrawLine(points[0], points[1]);
		DrawLine(points[1], points[2]);
		DrawLine(points[2], points[3]);
		DrawLine(points[3], points[0]);
	}

	//
	//
	//
	inline void DrawFillRect() {

	}

	//
	//
	//
	inline void DrawCircle(const Vector2& _center, float _radius) {
		
	}

	//
	//
	//
	inline void DrawFillCircle() {

	}

public: // Accesors & Mutators
	//
	// 
	//
	_NODISCARD inline Color getColor() const {
		return m_color;
	}

	//
	// 
	//
	inline void setColor(const Color& _color) {
		m_color = _color;
		SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, m_color.a);
	}

private: // Properties
	SDL_Renderer* m_renderer;

	Color m_color = Color::black();
};
LMK_END

#endif // LMK_HAVE_SDL
#endif // !LMK_GISMO_H_