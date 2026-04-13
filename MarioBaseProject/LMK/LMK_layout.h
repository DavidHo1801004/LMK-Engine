#pragma once
#ifndef _LMK_LAYOUT_H_
#define _LMK_LAYOUT_H_

#include "LMK_stdinc.h"
#include "LMK_gameobj.h"

LMK_BEGIN
// +--------------------------------------------------------------------------------+
// |					 															|
// | GRID LAYOUT			 														|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// A base class that defines a 2D rectangular grid layout.
//
class GridLayout2D : public Behaviour {
public: // Constructors & Destructors
	//
	// Create a new GridLayout2D with the given cellGap and cellSize.
	//
	inline GridLayout2D(Vector2 _cellGap, Vector2 _cellSize)
		: cellGap(_cellGap), cellSize(_cellSize) {}

	inline GridLayout2D() = default;

public: // Functions
	//
	// Converts a cell position to local space position.
	// 
	// @param _cellPosition:
	//		Cell position to convert.
	// 
	// @return 
	//		Local position of the center of the cell position.
	//
	[[nodiscard]] inline Vector2 CellToLocal(Vector2Int _cellPosition) const noexcept {
		return Vector2::Scale(_cellPosition, cellGap + cellSize);
	}

	//
	// Converts an interpolated cell position in floats to local space position.
	// 
	// Returns the local position in floats.
	// 
	// @param _cellPosition:
	//		Interpolated cell position to convert.
	// 
	// @return 
	//		Local position of the bottom-left of the cell position.
	//
	[[nodiscard]] inline Vector2 CellToLocalInterpolated(Vector2 _cellPosition) const noexcept {
		Vector2Int intCellPos = Vector2Int::FloorToInt(_cellPosition);
		return CellToLocal(intCellPos) + Vector2::Scale(_cellPosition - intCellPos, cellSize);
	}

	//
	// Converts a cell position to world space position.
	// 
	// @param _cellPosition:
	//		Cell position to convert.
	// 
	// @return
	//		World position of the bottom-left of the cell position.
	//
	[[nodiscard]] inline Vector2 CellToWorld(Vector2Int _cellPosition) const noexcept {
		return WorldToLocal(CellToLocal(_cellPosition));
	}

	//
	// Converts a local position to cell position.
	// 
	// @param _position:
	//		Local position to convert.
	// 
	// @return 
	//		Cell position of the local position.
	//
	[[nodiscard]] inline Vector2Int LocalToCell(Vector2 _localPosition) const noexcept {
		return Vector2Int::CeilToInt(Vector2{
			_localPosition.x / (cellSize.x + cellGap.x),
			_localPosition.y / (cellSize.y + cellGap.y)
		});
	}

	//
	// Converts a local position to cell position.
	// 
	// Returns the interpolated cell position in floats, rather than the exact cell position.
	// 
	// @param _localPosition:
	//		Local position to convert.
	// 
	// @return
	//		Interpolated cell position of the local position.
	//
	[[nodiscard]] inline Vector2 LocalToCellInterpolated(Vector2 _localPosition) const noexcept {
		return Vector2{
			_localPosition.x / (cellSize.x + cellGap.x),
			_localPosition.y / (cellSize.y + cellGap.y)
		};
	}

	//
	// Converts a world position to cell position.
	// 
	// @param _worldPosition:
	//		World position to convert.
	// 
	// @return
	//		Cell position of the world position.
	//
	[[nodiscard]] inline Vector2Int WorldToCell(Vector2 _worldPosition) const noexcept {
		return LocalToCell(WorldToLocal(_worldPosition));
	}

	//
	// Converts a world position to local position.
	// 
	// @param _worldPosition:
	//		World position to convert.
	// 
	// @return 
	//		Local position of the world position.
	//
	[[nodiscard]] inline Vector2 WorldToLocal(Vector2 _worldPosition) const noexcept {
		return transform->InverseTransformPoint(_worldPosition);
	}

	//
	// Converts a local position to world position.
	// 
	// @param _localPosition:
	//		Local position to convert.
	// 
	// @return 
	//		World position of the local position.
	//
	[[nodiscard]] inline Vector2 LocalToWorld(Vector2 _localPosition) const noexcept {
		return transform->TransformPoint(_localPosition);
	}

	//
	// Get the local bounds for a cell Get the location.
	// 
	// @param _cellPosition:
	//		Location of the cell.
	// 
	// @return
	//		Local bounds of cell Get the location.
	//
	[[nodiscard]] inline Bounds GetBoundsLocal(Vector2Int _cellPosition) const noexcept {
		return Bounds{
			CellToLocalInterpolated(_cellPosition) + Vector2(0.5f, 0.5f),
			cellSize
		};
	}
	
public: // Properties
	Vector2 cellGap;
	Vector2 cellSize;
};
LMK_END

#endif // !_LMK_LAYOUT_H_
