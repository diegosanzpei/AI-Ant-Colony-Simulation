#include "pch.hpp"
#include "FoodGrid.h"
#include "opengl.hpp"
#include "renderer.hpp"
#include "demo_simulation.hpp"

namespace cs380 {
void FoodGrid::Reset()
{
	for (int i = 0; i < FOOD_GRID_ROWS; ++i)
	{
		for (int j = 0; j < FOOD_GRID_COLS; ++j)
		{
			m_cells[i][j].m_food = 0;
		}
	}
}

FoodGrid::FoodGrid()
{
}

void FoodGrid::GetRowColumn(const glm::vec3& pos, int* row, int* col) const
{
	*row = (int)std::floor(pos.x / FOOD_CELL_WIDTH);
	*col = (int)std::floor(pos.z / FOOD_CELL_WIDTH);
}

void FoodGrid::DrawGrid(renderer* r)
{
	r->bind_mesh(renderer::mesh_type::quad, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	for (int i = 0; i < FOOD_GRID_ROWS; ++i)
	{
		for (int j = 0; j < FOOD_GRID_COLS; ++j)
		{
			if (m_cells[i][j].m_food > 0)
			{
				r->render_object(r->model_to_world(GetCoords(i, j), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(FOOD_CELL_HALF_WIDTH)));
			}
		}
	}
}

inline glm::vec3 FoodGrid::GetCoords(int row, int col) const
{
	return glm::vec3(row * FOOD_CELL_WIDTH + FOOD_CELL_HALF_WIDTH, 0.0f, FOOD_CELL_WIDTH * col + FOOD_CELL_HALF_WIDTH);
}

void FoodGrid::DropFood(const glm::vec3& pos)
{
	int wallRow, wallCol;
	g_demo.m_wallGrid.GetRowColumn(pos, &wallRow, &wallCol);
	bool cellHasWall = g_demo.m_wallGrid.CellHasWall(wallRow, wallCol);

	// Don't drop food in cells that have walls
	if (cellHasWall)
		return;

	// Get the position in the grid
	int row, col;
	GetRowColumn(pos, &row, &col);

	// Make sure it is in the grid
	if (row < 0 || col < 0 || row >= FOOD_GRID_ROWS || col >= FOOD_GRID_COLS)
		return;

	m_cells[row][col].m_food = MAX_FOOD_PER_CELL;
	return;

	//int r_start = row - FOOD_RADIUS;
	//int c_start = col - FOOD_RADIUS;
	//int r_end = row + FOOD_RADIUS;
	//int c_end = col + FOOD_RADIUS;
	//for (int r = r_start; r <= r_end; ++r)
	//{
	//	for (int c = c_start; c <= c_end; ++c)
	//	{
	//		if (r < 0 || c < 0 || r >= FOOD_GRID_ROWS || c >= FOOD_GRID_COLS)
	//			continue;
	//
	//		int diff_r = r - row;
	//		int diff_c = c - col;
	//
	//		if (diff_r * diff_r + diff_c * diff_c < FOOD_RADIUS * FOOD_RADIUS)
	//		{
	//			m_cells[r][c].m_food = MAX_FOOD_PER_CELL;
	//		}
	//	}
	//}
}
bool FoodGrid::CellHasFood(int row, int col)
{
	return m_cells[row][col].m_food != 0;
}
}