#pragma once
#ifndef LMK_GISMO_H_
#define LMK_GISMO_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL2
#include "LMK_coremdl.h"

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
	inline void DrawRect(const Rect& _rect) {
		const SDL_FPoint points[5] = {
			_rect.minPos(),
			Vector2(_rect.xMax(), _rect.yMin()),
			_rect.maxPos(),
			Vector2(_rect.xMin(), _rect.yMax()),
			_rect.minPos()
		};
		SDL_RenderDrawLinesF(m_renderer, points, 5);
	}

	//
	// 
	//
	inline void DrawFillRect(const Rect& _rect) {

	}

	//
	//
	//
	inline void DrawCircle(const Vector2& _center, float _radius) {
		
	}

	//
	//
	//
	inline void DrawFillCircle(const Vector2& _center, float _radius) {

	}
	
	//
	// 
	//
	inline void DrawPolygon(const std::vector<Vector2>& _vertices) {
		Vector2 start, end;
		for (size_t i = 0; i < _vertices.size() - 1; i++) {
			start = _vertices[i];
			end = _vertices[i + 1];
			DrawLine(start, end);
		}
		DrawLine(end, _vertices[0]);
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
	inline void SetColor(const Color& _color) {
		m_color = _color;
		SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, m_color.a);
	}

private: // Properties
	SDL_Renderer* m_renderer;

	Color m_color = Color::black();
};
LMK_END

#endif // LMK_HAVE_SDL2
#endif // !LMK_GISMO_H_