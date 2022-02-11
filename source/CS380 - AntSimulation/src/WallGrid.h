#pragma once
#include "global.h"
#include "math.hpp"

namespace cs380 {
struct WallCell
{
	bool m_hasWall = false;
};

class renderer;
class WallGrid
{
public:

	void Reset();
	WallGrid();
	void GetRowColumn(const glm::vec3& pos, int* row, int* col) const;

	void DrawGrid(renderer* r);
	inline glm::vec3 GetCoords(int row, int col) const;

	void PlaceWall(const glm::vec3& pos);
	void RemoveWall(const glm::vec3& pos);

	bool CellHasWall(int row, int col);

	inline WallCell& GetCell(int row, int col) { return m_cells[row][col]; };

private:
	WallCell m_cells[WALL_GRID_ROWS][WALL_GRID_COLS];
};
}