#pragma once
#include "math.hpp"
#include "global.h"

namespace cs380
{
	struct PheromoneCell
	{
		bool m_foodPheromone = false;
		bool m_nestPheromone = false;
		float m_foodTimer = 0.0f;
		float m_nestTimer = 0.0f;

		

		//float m_foodIntensity = 0.0f;
		//float m_nestIntensity = 0.0f;
	};
	class renderer;
	class PheromoneGrid
	{
	public:

		void Reset();

		PheromoneGrid();

		void DrawGrid(renderer* r);

		glm::vec3 GetCoords(int row, int col) const;

		void Evaporation();

		void DropFoodPheromone(int row, int col);//, float value);
		void DropNestPheromone(int row, int col);//, float value);

		void GetRowColumn(const glm::vec3& pos, int* row, int* col);

		inline PheromoneCell& GetCell(int row, int col) { return m_cells[row][col]; }

		float get_food_intensity(int row, int col);
		float get_nest_intensity(int row, int col);

		float m_pheromoneTime = 4.0f;

	private:
		PheromoneCell m_cells[PHEROMONE_GRID_ROWS][PHEROMONE_GRID_COLS];
	};
}