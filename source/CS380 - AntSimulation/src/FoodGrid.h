#pragma once
#include "global.h"
#include "math.hpp"

namespace cs380 {
struct FoodCell
{
	unsigned char m_food = 0;
};

class renderer;
class FoodGrid
{
public:
	void Reset();
	FoodGrid();
	void GetRowColumn(const glm::vec3& pos, int* row, int* col) const;

	void DrawGrid(renderer* r);
	inline glm::vec3 GetCoords(int row, int col) const;

	void DropFood(const glm::vec3& pos);

	bool CellHasFood(int row, int col);

	inline FoodCell& GetCell(int row, int col) { return m_cells[row][col]; };

private:
	FoodCell m_cells[FOOD_GRID_ROWS][FOOD_GRID_COLS];
};
}