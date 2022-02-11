#include "pch.hpp"
#include "PheromoneGrid.h"
#include "opengl.hpp"
#include "renderer.hpp" 
#include "demo_simulation.hpp"

namespace cs380 {

float PheromoneGrid::get_food_intensity(int row, int col)
{
	float intensity = 1.0f - glm::min(1.0f, ((float)glfwGetTime() - m_cells[row][col].m_foodTimer) / m_pheromoneTime);
	if (Near(intensity, 0.0f) || intensity < 0.0f)
		return 0.0f;
	else
		return intensity;
}
float PheromoneGrid::get_nest_intensity(int row, int col)
{
	float intensity = 1.0f - glm::min(1.0f, ((float)glfwGetTime() - m_cells[row][col].m_nestTimer) / m_pheromoneTime);
	if (Near(intensity, 0.0f) || intensity < 0.0f)
		return 0.0f;
	else
		return intensity;
}

void PheromoneGrid::Reset()
{
	for (int i = 0; i < PHEROMONE_GRID_ROWS; ++i)
	{
		for (int j = 0; j < PHEROMONE_GRID_COLS; ++j)
		{
			m_cells[i][j].m_foodPheromone = false;
			m_cells[i][j].m_nestPheromone = false;
			m_cells[i][j].m_foodTimer = 0.0f;
			m_cells[i][j].m_nestTimer = 0.0f;
		}
	}
}

PheromoneGrid::PheromoneGrid()
{
}

void PheromoneGrid::DrawGrid(renderer* r)
{
	r->bind_mesh(renderer::mesh_type::quad, glm::vec4(0.0f));
	glm::vec4 foodColor = glm::vec4(0.0f, 0.7f, 0.0f, 1.0f);
	glm::vec4 nestColor = glm::vec4(0.0f, 0.0f, 0.7f, 1.0f);
	for (int i = 0; i < PHEROMONE_GRID_ROWS; ++i)
	{
		for (int j = 0; j < PHEROMONE_GRID_COLS; ++j)
		{
			// Only draw if either of the pheromones is active
			if (m_cells[i][j].m_foodPheromone || m_cells[i][j].m_nestPheromone)
			{
				// Normalize the intensity
				float foodIntensity = m_cells[i][j].m_foodPheromone ? get_food_intensity(i, j) : 0.0f;//m_cells[i][j].m_foodIntensity / g_demo.m_maxPheromoneIntensity;											  _pheromoneTime
				float nestIntensity = m_cells[i][j].m_nestPheromone ? get_nest_intensity(i, j) : 0.0f;//m_cells[i][j].m_nestIntensity / g_demo.m_maxPheromoneIntensity;

				// Get each color multiplied by the normalized intensity
				r->set_color(foodIntensity * foodColor + nestIntensity * nestColor);

				// Combine both colors to draw the cube
				r->render_object(r->model_to_world(GetCoords(i, j), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(PHEROMONE_HALF_CELL_WIDTH)));
			}
		}
	}
}

glm::vec3 PheromoneGrid::GetCoords(int row, int col) const
{
	return glm::vec3(row * PHEROMONE_CELL_WIDTH + PHEROMONE_HALF_CELL_WIDTH, 0.0f, PHEROMONE_CELL_WIDTH * col + PHEROMONE_HALF_CELL_WIDTH);
}

void PheromoneGrid::Evaporation()
{
	// For each row
	for (int row = 0; row < PHEROMONE_GRID_ROWS; ++row)
	{
		// For each cell
		for (int col = 0; col < PHEROMONE_GRID_COLS; ++col)
		{
			if (m_cells[row][col].m_nestPheromone && get_nest_intensity(row, col) == 0.0f)
				m_cells[row][col].m_nestPheromone = false;

			else if (m_cells[row][col].m_foodPheromone && get_food_intensity(row, col) == 0.0f)
				m_cells[row][col].m_foodPheromone = false;
		}
	}
}

void PheromoneGrid::DropFoodPheromone(int row, int col)//, float value)
{
	m_cells[row][col].m_foodPheromone = true;
	m_cells[row][col].m_foodTimer = (float)glfwGetTime();
	//m_cells[row][col].m_foodIntensity += value;

	//if (m_cells[row][col].m_foodIntensity > (float)g_demo.m_maxPheromoneIntensity)
	//	m_cells[row][col].m_foodIntensity = (float)g_demo.m_maxPheromoneIntensity;
}

void PheromoneGrid::DropNestPheromone(int row, int col)//, float value)
{
	m_cells[row][col].m_nestPheromone = true;
	m_cells[row][col].m_nestTimer = (float)glfwGetTime();
	//m_cells[row][col].m_nestIntensity += value;

	//if (m_cells[row][col].m_nestIntensity > (float)g_demo.m_maxPheromoneIntensity)
	//	m_cells[row][col].m_nestIntensity = (float)g_demo.m_maxPheromoneIntensity;
}

void PheromoneGrid::GetRowColumn(const glm::vec3& pos, int* row, int* col)
{
	*row = (int)std::floor(pos.x / PHEROMONE_CELL_WIDTH);
	*col = (int)std::floor(pos.z / PHEROMONE_CELL_WIDTH);
}
}