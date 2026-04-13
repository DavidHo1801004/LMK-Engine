#pragma once
#ifndef _LMK_TILE_MAP_H_
#define _LMK_TILE_MAP_H_

#include "LMK_stdinc.h"
#include "LMK_layout.h"
#include "LMK_gameobj.h"
#include "LMK_physics2d.h"
#include "LMK_render2d.h"

LMK_BEGIN


// +--------------------------------------------------------------------------------+
// |					 															|
// | TILE DATA						 												|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// A structure contains the required data for rendering a Tile.
//
struct TileData {
	// Color of the Tile.
	Color color;	
	// Sprite to be rendered Get the Tile.
	Sprite sprite;
	// Transform matrix of the Tile.
	Matrix3x3 transformMatrix;
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | TILE BASE						 												|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// Class for a default tile in the TileMap.
// 
// 
//
class Tile {


public: // Functions
	//
	// 
	//
	[[nodiscard]] inline TileData GetTileData() const noexcept {
		return { color, sprite, transformMatrix };
	}

public: // Properties
	Color color;
	Sprite sprite;
	Matrix3x3 transformMatrix;
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | TILE MAP PALETTE				 												|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// 
//
class TileMapPalette {
public: // Typedef
	// 
	using storage_type	= std::unordered_map<int, TileData>;
	// 
	using key_type		= typename storage_type::key_type;
	// 
	using value_type	= typename storage_type::value_type::second_type;
	//
	using size_type		= typename storage_type::size_type;

public: // Functions
	//
	//
	//
	inline TileMapPalette() = default;

public: // Operators Overloads
	[[nodiscard]] inline TileData operator[](int _index) const {
		if (m_palette.find(_index) == m_palette.end())
			return TileData();
		else 
			return m_palette.at(_index);
	}

public: // Functions
	//
	// 
	//
	inline void Add(int _index, const TileData& _newTile) {
		m_palette.emplace(_index, _newTile);
	}

	//
	// 
	//
	inline void Remove(int _index) {
		m_palette.erase(_index);
	}

	//
	// 
	//
	[[nodiscard]] inline size_type Count() const noexcept {
		return m_palette.size();
	}

	//
	//
	//
	[[nodiscard]] inline const value_type Get(key_type _index) const {
		return m_palette.at(_index);
	}

private:
	storage_type m_palette;
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | TILE MAP				 														|
// |					 															|
// +--------------------------------------------------------------------------------+

//
//
//
class TileMap : public GridLayout2D {

	//
	// IMPORTANT:
	// This is only a temporary implementation of this class.
	//

public: // Typedef
	// 
	using storage_type	= std::vector<std::vector<TileData>>;

public: // Public
	//
	// 
	//
	inline void GenerateTileMap(std::string _filePath) {
		std::ifstream file(_filePath);
		std::string line;

		m_tileMap.clear();

		while (getline(file, line)) {
			std::vector<TileData> row;
			int tileIndex;
			std::istringstream iss(line);

			while (iss >> tileIndex) {
				std::cout << tileIndex << " ";
				row.push_back(m_palette->Get(tileIndex));
			}

			if (!row.empty()) {
				std::cout << "\n";
				m_tileMap.push_back(row);
			}
		}

		file.close();
	}

	//
	// Generate a new tile map with the given _tileMapData.
	// 
	// @param _tileMapData:
	//		A 2D array of intergers indicating which value of the current tile palette
	//		to generate Get a given tile.
	//		If the value of a tile in _tileMapData exceed the number of values in the current
	//		tile palette, a value of 0 (nothing) will be used.
	// 
	// @return
	//		Was the generation process successful?
	//
	inline bool GenerateTileMap(std::vector<std::vector<int>> _tileMapData) {
		for (size_t y = 0; y < _tileMapData.size(); y++) {
			for (size_t x = 0; x < _tileMapData.size(); x++) {
				m_tileMap[x][y] = m_palette->Get(_tileMapData[x][y]);
			}
		}
	}

private:
	inline void UpdateTileMap() {

	}

public: // Accessors
	//
	[[nodiscard]] inline const TileMapPalette& palette() const noexcept {
		return *m_palette;
	}

public: // Mutators
	//
	inline void setPalette(TileMapPalette& _newPalette) noexcept {
		m_palette = &_newPalette;
		UpdateTileMap();
	}

private: // Properties
	storage_type m_tileMap;

	const TileMapPalette* m_palette;
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | TILE MAP RENDERER				 												|
// |					 															|
// +--------------------------------------------------------------------------------+

//
//
//
class TileMapRenderer final : public Renderer {
public:
	inline void Render(const Camera2D& _camera) const override {

	}
};
LMK_END

#endif // !_LMK_TILE_MAP_H_
