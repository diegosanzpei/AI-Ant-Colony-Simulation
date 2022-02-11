#include "pch.hpp"
#include "opengl.hpp"
#include "renderer.hpp"
#include "WallGrid.h"

namespace cs380 {
void WallGrid::Reset()
{
	for (int i = 0; i < WALL_GRID_ROWS; ++i)
	{
		for (int j = 0; j < WALL_GRID_COLS; ++j)
		{
			m_cells[i][j].m_hasWall = false;
		}
	}
}

WallGrid::WallGrid()
{
}

void WallGrid::GetRowColumn(const glm::vec3& pos, int* row, int* col) const
{
	*row = (int)std::floor(pos.x / WALL_CELL_WIDTH);
	*col = (int)std::floor(pos.z / WALL_CELL_WIDTH);
}

void WallGrid::DrawGrid(renderer* r)
{
	r->bind_mesh(renderer::mesh_type::pheromone, glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
	for (int i = 0; i < WALL_GRID_ROWS; ++i)
	{
		for (int j = 0; j < WALL_GRID_COLS; ++j)
		{
			if (m_cells[i][j].m_hasWall)
			{
				r->render_object(r->model_to_world(GetCoords(i, j), glm::vec3(0.0f), glm::vec3(WALL_CELL_WIDTH)));
			}
		}
	}
}

inline glm::vec3 WallGrid::GetCoords(int row, int col) const
{
	return glm::vec3(row * WALL_CELL_WIDTH + WALL_CELL_HALF_WIDTH, 0.0f, WALL_CELL_WIDTH * col + WALL_CELL_HALF_WIDTH);
}

void WallGrid::PlaceWall(const glm::vec3& pos)
{
	// Get the position in the grid
	int row, col;
	GetRowColumn(pos, &row, &col);

	// Make sure it is in the grid
	if (row < 0 || col < 0 || row >= WALL_GRID_ROWS || col >= WALL_GRID_COLS)
		return;

	m_cells[row][col].m_hasWall = true;
}

void WallGrid::RemoveWall(const glm::vec3& pos)
{
	// Get the position in the grid
	int row, col;
	GetRowColumn(pos, &row, &col);

	// Make sure it is in the grid
	if (row < 0 || col < 0 || row >= WALL_GRID_ROWS || col >= WALL_GRID_COLS)
		return;

	m_cells[row][col].m_hasWall = false;
}

bool WallGrid::CellHasWall(int row, int col)
{
	return m_cells[row][col].m_hasWall != 0;
}
}