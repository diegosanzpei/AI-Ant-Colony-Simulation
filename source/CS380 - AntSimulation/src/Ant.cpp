#include "pch.hpp"
#include "Ant.h"
#include "demo_simulation.hpp"
#include "global.h"

namespace cs380 {
Ant::Ant()
	: m_pos(FLOOR_SCALE_X / 2.0f, ANTS_Y, FLOOR_SCALE_Z / 2.0f)
	, m_forward(0.0f, 0.0f, 0.0)
	, m_speed(1.0f)
	, m_force(0.0f, 0.0f, 0.0f)
	, m_desiredDir(0.0f)
{
}

void Ant::update()
{
	//assert(m_pos.y == ANTS_Y);
	//assert(m_speed < 1.0f);

	world_edges();
	leave_food();
	get_food();

	// Ant is carrying food, look for home or corresponding pheromones
	if (m_hasFood)
	{
		// Check if ant found home
		if (look_for_nest(m_desiredDir) || look_for_nest_pheromones(m_desiredDir))
		{
			glm::vec3 steeringForce = (m_desiredDir - m_forward) * 10.0f;
			add_force(steeringForce);
			m_isFollowingPheromone = true;
		}
		else
		{
			wander();
			m_isFollowingPheromone = false;
		}
	}
	
	// Otherwise, look for food
	else
	{
		// Check if ant found food or food pheromones
		if (look_for_food(m_desiredDir) || look_for_food_pheromones(m_desiredDir))
		{
			glm::vec3 steeringForce = (m_desiredDir - m_forward) * 10.0f;
			add_force(steeringForce);
			m_isFollowingPheromone = true;
		}
		// If not, wander
		else
		{
			wander();
			m_isFollowingPheromone = false;
		}
	}

	avoid_walls();
	integrate();
}

void Ant::integrate()
{
	float dt = 1/60.0f;

	m_forward += m_force * dt;
	m_forward = glm::normalize(m_forward);
	m_pos += m_forward * m_speed * dt;
	m_force = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Ant::add_force(const glm::vec3& force)
{
	m_force += force;
}

glm::vec3 Ant::pos() const
{
	return m_pos;
}

glm::vec3 Ant::forward() const
{
	return m_forward;
}

float Ant::speed() const
{
	return m_speed;
}

float Ant::radius() const
{
	return m_radius;
}

bool Ant::has_food() const
{
	return m_hasFood;
}

void Ant::reset(const glm::vec3& pos)
{
	m_pos = pos;
	m_radius = rand_float_in_range(0.1f, 0.2f);
	m_speed = rand_float_in_range(0.4f, 0.8f);
	m_currentAngle = (float)(rand() % 360);
	m_targetAngle = (float)(rand() % 360);
}


void Ant::world_edges()
{
	if (m_pos.x > FLOOR_SCALE_X - WORLD_EDGE)
		reflect(glm::vec3(-1.0f, 0.0f, 0.0f));
	else if (m_pos.x < WORLD_EDGE)
		reflect(glm::vec3(1.0f, 0.0f, 0.0f));
	if (m_pos.z > FLOOR_SCALE_Z - WORLD_EDGE)
		reflect(glm::vec3(0.0f, 0.0f, -1.0f));
	else if (m_pos.z < WORLD_EDGE)
		reflect(glm::vec3(0.0f, 0.0f, 1.0f));
}

void Ant::avoid_walls()
{
	int row, col;
	g_demo.m_wallGrid.GetRowColumn(m_pos, &row, &col);

	if (row < 0 || col < 0 || row >= WALL_GRID_ROWS || col >= WALL_GRID_COLS)
		return;

	if (g_demo.m_wallGrid.CellHasWall(row, col))
	{
		reflect_cell(row, col);
		return;
	}

	int next_row, next_col;
	g_demo.m_wallGrid.GetRowColumn(m_pos + m_forward * WALL_CELL_HALF_WIDTH, &next_row, &next_col);

	// Check if there is a wall
	if (g_demo.m_wallGrid.CellHasWall(next_row, next_col))
	{
		reflect_cell(next_row, next_col);
		return;
	}

	if (row == next_row || col == next_col)
		return;

	int r_off = next_row - row;
	int c_off = next_col - col;

	int n_idx;

	if (c_off > 0.0f)
	{
		if (r_off > 0.0f)
			n_idx = 2;
		else
			n_idx = 0;
	}
	else
	{
		if (r_off > 0.0f)
			n_idx = 4;
		else
			n_idx = 6;
	}

	int next_idx = n_idx + 1;
	int prev_idx = n_idx == 0 ? 7 : n_idx - 1;

	// Check diagonals
	if (g_demo.m_wallGrid.CellHasWall(row + child_r_offset[next_idx], col + child_c_offset[next_idx]))
		reflect_cell(row + child_r_offset[next_idx], col + child_c_offset[next_idx]);

	else if (g_demo.m_wallGrid.CellHasWall(row + child_r_offset[prev_idx], col + child_c_offset[prev_idx]))
		reflect_cell(row + child_r_offset[prev_idx], col + child_c_offset[prev_idx]);
}

void Ant::leave_food()
{
	//if (!m_hasFood)
	//	return;

	const demo& demo = g_demo;

	// Get the ant row and column in the food grid
	int antRow, antCol;
	demo.m_foodGrid.GetRowColumn(m_pos, &antRow, &antCol);

	if (antRow < 0 || antCol < 0 || antRow >= FOOD_GRID_ROWS || antCol >= FOOD_GRID_COLS)
		return;

	// Get the nest row and column in the food grid
	int nestRow, nestCol;
	demo.m_foodGrid.GetRowColumn(demo.m_nestPos, &nestRow, &nestCol);

	if (nestRow < 0 || nestCol < 0 || nestRow >= FOOD_GRID_ROWS || nestCol >= FOOD_GRID_COLS)
		return;

	int minRow = nestRow - NEST_RADIUS;
	int minCol = nestCol - NEST_RADIUS;
	int maxRow = nestRow + NEST_RADIUS;
	int maxCol = nestCol + NEST_RADIUS;

	// If out of the square, nothing to leave since ant isn't at nest
	if (antRow < minRow || antCol < minCol || antRow > maxRow || antCol > maxCol)
		return;

	int dy = antCol - nestCol;
	int dx = antRow - nestRow;

	// If ant within the square, and distance to nest is less than the radius, drop the food
	if (dx * dx + dy * dy < NEST_RADIUS_SQ)
	{
		if (m_hasFood)
		{
			m_hasFood = false;
			m_forward *= -1.0f;
		}

		// Even if it wasn't carrying food, we need to reset the pheromones to drop to max intensity
		m_pheromoneCtr = g_demo.m_pheromonesPerAnt;
	}
}

void Ant::get_food()
{
	//if (m_hasFood)
	//	return;

	demo& demo = g_demo;

	// Get the row and column of the ant in the food grid
	int row, col;
	demo.m_foodGrid.GetRowColumn(m_pos, &row, &col);

	if (row < 0 || col < 0 || row >= FOOD_GRID_ROWS || col >= FOOD_GRID_COLS)
		return;

	// If the cell the ant is in has food
	if (demo.m_foodGrid.CellHasFood(row, col))
	{
		// Get the food if the ant already doesn't have food
		if (!m_hasFood)
		{
			demo.m_foodGrid.GetCell(row, col).m_food -= 1;
			m_hasFood = true;
			m_forward *= -1.0f;
		}

		// Even if the ant has food, we need to reset the pheromones to drop to max intensity
		m_pheromoneCtr = g_demo.m_pheromonesPerAnt;
	}
}

void Ant::wander()
{
	glm::vec3 desiredPos = point_in_circunference();
	glm::vec3 desiredVec = desiredPos - m_pos;
	//D3DXVec3Normalize(&desiredVec, &desiredVec);
	glm::vec3 steeringForce = (desiredVec - m_forward) * 50.0f;
	add_force(steeringForce);
}

bool Ant::look_for_food(glm::vec3& dir)
{
	demo& demo = g_demo;
	//////////////////////LOOK FOR FOOD///////////////////////////
	int row, col;
	demo.m_foodGrid.GetRowColumn(m_pos, &row, &col);

	if (row < 0 || col < 0 || row >= FOOD_GRID_ROWS || col >= FOOD_GRID_COLS)
		return false;

	int r_start = row - demo.m_antFovRadius;
	int r_end = row + demo.m_antFovRadius;
	int c_start = col - demo.m_antFovRadius;
	int c_end = col + demo.m_antFovRadius;

	bool foundFood = false;
	int minFoodDist = (std::numeric_limits<int>::max)();

	for (int r = r_start; r <= r_end; ++r)
	{
		for (int c = c_start; c <= c_end; ++c)
		{
			if (r < 0 || c < 0 || r >= FOOD_GRID_ROWS || c >= FOOD_GRID_COLS)
				continue;

			// Cell has no food
			if (demo.m_foodGrid.CellHasFood(r, c) == false)
				continue;
			
			glm::vec3 d((float)(r - row), 0.0f, (float)(c - col));
			
			// Not in FOV
			if (glm::dot(m_forward, d) < 0.0f)
				continue;
			
			int dist = (int)glm::length2(d);

			// Cell is in distance and is closer than previous food
			if (dist <= demo.m_antFovRadius * demo.m_antFovRadius && dist < minFoodDist)
			{
				if (path_is_clear(demo.m_foodGrid.GetCoords(r, c)) == false)
					continue;

				foundFood = true;
				minFoodDist = dist;
				dir = d;
			}
			
		}
	}
	//////////////////////LOOK FOR FOOD///////////////////////////
	// If found food, the ant should go there
	// dir contains the desired direction
	return foundFood;
}

bool Ant::look_for_food_pheromones(glm::vec3& dir)
{
	demo& demo = g_demo;
	if (!demo.m_lookForPheromones && m_isFollowingPheromone)
	{
		return true;
	}

	// Get the row and column of the current ant
	int antRow, antCol;
	demo.m_pheromoneGrid.GetRowColumn(m_pos, &antRow, &antCol);

	if (antRow < 0 || antCol < 0 || antRow >= PHEROMONE_GRID_ROWS || antCol >= PHEROMONE_GRID_COLS)
		return false;

	// Get the starting min (row, col) and max (row, col) of the square of side ANT_FOV_RADIUS that we will explore
	int r_start = antRow - demo.m_antFovRadius;
	int r_end = antRow + demo.m_antFovRadius;
	int c_start = antCol - demo.m_antFovRadius;
	int c_end = antCol + demo.m_antFovRadius;

	bool foundFoodPheromone = false;
	double totalPheromoneFactor = 0.0f;
	float pheromoneNW = 0.0f;
	float pheromoneN = 0.0f;
	float pheromoneNE = 0.0f;
	float pheromoneS = 0.0f;

	for (int r = r_start; r <= r_end; ++r)
	{
		for (int c = c_start; c <= c_end; ++c)
		{
			if (r < 0 || c < 0 || r >= PHEROMONE_GRID_ROWS || c >= PHEROMONE_GRID_COLS)
				continue;

			// Cell has no food pheromones
			if (demo.m_pheromoneGrid.GetCell(r, c).m_foodPheromone == false)
				continue;

			glm::vec3 antToCell((float)(r - antRow), 0.0f, (float)(c - antCol));
			antToCell = glm::normalize(antToCell);

			float dot = glm::dot(m_forward, antToCell);

			int distSq = (int)glm::length2(antToCell);

			// Cell is within ant FOV
			if (distSq <= demo.m_antFovRadius * demo.m_antFovRadius)
			{
				if (path_is_clear(demo.m_pheromoneGrid.GetCoords(r, c)) == false)
					continue;

				// Add to the total intensity
				foundFoodPheromone = true;
				float foodIntensity = demo.m_pheromoneGrid.get_food_intensity(r, c);
				//totalPheromone += foodIntensity;

				if (dot < 0.0f)
				{
					pheromoneS += foodIntensity;
					continue;
				}

				float angle = glm::degrees(acos(dot));

				// Check in which section of the FOV, the current cell is, and add the intensity accordingly
				if (angle < 30.0f)
					pheromoneN += foodIntensity;
				else
				{
					float crossY = m_forward.z * antToCell.x - m_forward.x * antToCell.z;

					if (crossY > 0.0f)
						pheromoneNW += foodIntensity;
					else
						pheromoneNE += foodIntensity;
				}
			}
		}
	}

	if (!foundFoodPheromone)
		return false;

	// NW
	if (pheromoneNW > pheromoneN && pheromoneNW > pheromoneNE)// && pheromoneNW > pheromoneS)
	{
		// Rotate the forward to obtain the desired direction
		glm::mat4 rotationY = glm::rotate(glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	}
	// N
	else if (pheromoneN > pheromoneNW && pheromoneN > pheromoneNE)// && pheromoneN > pheromoneS)
	{
		dir = m_forward;
	}
	// NE
	else if (pheromoneNE > pheromoneNW && pheromoneNE > pheromoneN)// && pheromoneNE > pheromoneS)
	{
		// Rotate the forward to obtain the desired direction
		glm::mat4 rotationY = glm::rotate(glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	}
	else if (pheromoneS > pheromoneNW && pheromoneS > pheromoneN)// && pheromoneS > pheromoneNE)
	{
		// Rotate the forward to obtain the desired direction
		glm::mat4 rotationY = glm::rotate(glm::radians(178.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	}

	//int exp = demo.m_pheromoneExponent;
	//
	//double pheromoneNWFactor = std::pow(pheromoneNW, exp);
	//double pheromoneNFactor = std::pow(pheromoneN, exp);
	//double pheromoneNEFactor = std::pow(pheromoneNE, exp);
	//
	//totalPheromoneFactor = std::pow(pheromoneNW, exp) + std::pow(pheromoneN, exp) + std::pow(pheromoneNE, exp);
	//
	//// Make a decision based on probability
	//int factor = 100000;
	//float pNW = (float)(std::pow(pheromoneNW, exp) / totalPheromoneFactor) * factor;
	//float pN = (float)(std::pow(pheromoneN, exp) / totalPheromoneFactor) * factor;
	//float pNE = (float)(std::pow(pheromoneNE, exp) / totalPheromoneFactor) * factor;
	//
	//int random = (rand() % factor) + 1;
	//
	//// North-West
	//if (random > 0 && (float)random < pNW)
	//{
	//	// Rotate the forward to obtain the desired direction
	//	glm::mat4 rotationY = glm::rotate(glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//	dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	//}
	//// North
	//else if ((float)random >= pNW && (float)random < (pNW + pN))
	//	dir = m_forward;
	//// North-East
	//else
	//{
	//	// Rotate the forward to obtain the desired direction
	//	glm::mat4 rotationY = glm::rotate(glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//	dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	//}

	// Return true to stop wandering as we have found food pheromone
	return true;
}

bool Ant::look_for_nest(glm::vec3& dir)
{
	demo& demo = g_demo;
	int nestRow, nestCol;
	demo.m_foodGrid.GetRowColumn(demo.m_nestPos, &nestRow, &nestCol);

	if (nestRow < 0 || nestCol < 0 || nestRow >= FOOD_GRID_ROWS || nestCol >= FOOD_GRID_COLS)
		return false;

	int antRow, antCol;
	demo.m_foodGrid.GetRowColumn(m_pos, &antRow, &antCol);

	if (antRow < 0 || antCol < 0 || antRow >= FOOD_GRID_ROWS || antCol >= FOOD_GRID_COLS)
		return false;


	glm::vec3 antToCell((float)(nestRow - antRow), 0.0f, (float)(nestCol - antCol));

	// Not in FOV
	if (glm::dot(m_forward, antToCell) < 0.0f)
		return false;

	int dist = (int)glm::length2(antToCell);

	// Cell is in distance and is closer than previous nest cell
	if (dist <= demo.m_antFovRadius * demo.m_antFovRadius)
	{
		if (path_is_clear(demo.m_foodGrid.GetCoords(nestRow, nestCol)) == false)
		{
			return false;
		}


		//std::cout << "found nest" << std::endl;

		dir = antToCell;
		return true;
	}
	return false;
}

bool Ant::look_for_nest_pheromones(glm::vec3& dir)
{
	demo& demo = g_demo;
	if (!demo.m_lookForPheromones && m_isFollowingPheromone)
	{
		return true;
	}

	// Get the row and column of the current ant
	int antRow, antCol;
	demo.m_pheromoneGrid.GetRowColumn(m_pos, &antRow, &antCol);

	if (antRow < 0 || antCol < 0 || antRow >= PHEROMONE_GRID_ROWS || antCol >= PHEROMONE_GRID_COLS)
		return false;

	// Get the starting min (row, col) and max (row, col) of the square of side ANT_FOV_RADIUS that we will explore
	int r_start = antRow - demo.m_antFovRadius;
	int r_end = antRow + demo.m_antFovRadius;
	int c_start = antCol - demo.m_antFovRadius;
	int c_end = antCol + demo.m_antFovRadius;

	bool foundNestPheromone = false;
	double totalPheromoneFactor = 0.0f;
	float pheromoneNW = 0.0f;
	float pheromoneN = 0.0f;
	float pheromoneNE = 0.0f;
	float pheromoneS = 0.0f;

	for (int r = r_start; r <= r_end; ++r)
	{
		for (int c = c_start; c <= c_end; ++c)
		{
			if (r < 0 || c < 0 || r >= PHEROMONE_GRID_ROWS || c >= PHEROMONE_GRID_COLS)
				continue;

			// Cell has no nest pheromones
			if (demo.m_pheromoneGrid.GetCell(r, c).m_nestPheromone == false)
			{
				continue;
			}

			glm::vec3 antToCell((float)(r - antRow), 0.0f, (float)(c - antCol));

			int distSq = (int)glm::length2(antToCell);

			// Cell is within ant FOV
			if (distSq <= demo.m_antFovRadius * demo.m_antFovRadius)
			{
				if (path_is_clear(demo.m_pheromoneGrid.GetCoords(r, c)) == false)
					continue;

				// Add to the total intensity
				foundNestPheromone = true;
				float nestIntensity = demo.m_pheromoneGrid.get_nest_intensity(r, c);// glm::length2(g_demo.m_nestPos - m_pos);
				//totalPheromone += nestIntensity;

				// South
				float dot = glm::dot(m_forward, antToCell);
				if (dot < 0.0f)
				{
					pheromoneS += nestIntensity;
					continue;
				}

				float angle = glm::degrees(acos(dot));

				// Check in which section of the FOV, the current cell is, and add the intensity accordingly
				if (angle < 30.0f)
					pheromoneN += nestIntensity;
				else
				{
					float crossY = m_forward.z * antToCell.x - m_forward.x * antToCell.z;

					if (crossY > 0.0f)
						pheromoneNW += nestIntensity;
					else
						pheromoneNE += nestIntensity;
				}
			}
		}
	}

	if (!foundNestPheromone)
		return false;


	//std::cout << "found pheromones" << std::endl;

	// NW
	if (pheromoneNW > pheromoneN && pheromoneNW > pheromoneNE)// && pheromoneNW > pheromoneS)
	{
		// Rotate the forward to obtain the desired direction
		glm::mat4 rotationY = glm::rotate(glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	}
	// N
	else if (pheromoneN > pheromoneNW && pheromoneN > pheromoneNE)// && pheromoneN > pheromoneS)
	{
		dir = m_forward;
	}
	// NE
	else if (pheromoneNE > pheromoneNW && pheromoneNE > pheromoneN)// && pheromoneNE > pheromoneS)
	{
		// Rotate the forward to obtain the desired direction
		glm::mat4 rotationY = glm::rotate(glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	}
	else if (pheromoneS > pheromoneNW && pheromoneS > pheromoneN && pheromoneS > pheromoneNE)
	{
		// Rotate the forward to obtain the desired direction
		glm::mat4 rotationY = glm::rotate(glm::radians(178.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	}

	//int exp = demo.m_pheromoneExponent;
	//
	//double pheromoneNWFactor = std::pow(pheromoneNW, exp);
	//double pheromoneNFactor = std::pow(pheromoneN, exp);
	//double pheromoneNEFactor = std::pow(pheromoneNE, exp);
	//
	//totalPheromoneFactor = pheromoneNWFactor + pheromoneNFactor + pheromoneNEFactor;
	//
	//// Make a decision based on probability
	//int factor = 100000;
	//float pNW = (float)(pheromoneNWFactor / totalPheromoneFactor) * factor;
	//float pN = (float)(pheromoneNFactor / totalPheromoneFactor) * factor;
	//float pNE = (float)(pheromoneNEFactor / totalPheromoneFactor) * factor;
	//
	//int random = (rand() % factor) + 1;
	//
	//// North-West
	//if (random > 0 && (float)random < pNW)
	//{
	//	// Rotate the forward to obtain the desired direction
	//	glm::mat4 rotationY = glm::rotate(glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//	dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	//}
	//// North
	//else if ((float)random >= pNW && (float)random < (pNW + pN))
	//	dir = m_forward;
	//// North-East
	//else
	//{
	//	// Rotate the forward to obtain the desired direction
	//	glm::mat4 rotationY = glm::rotate(glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//	dir = glm::vec3(rotationY * glm::vec4(dir, 0.0f));
	//}
	//std::cout << "found some" << std::endl;
	// Return true to stop wandering as we have found nest pheromone
	return true;
}

void Ant::reflect(const glm::vec3& normal)
{
	// Already reflected
	if (glm::dot(normal, m_forward) >= 0.0f)
		return;

	m_forward = glm::reflect(m_forward, normal);
	m_force = m_forward;
	m_targetAngle = glm::degrees(atan2(m_forward.x, m_forward.z));
	if (m_targetAngle < 0.0f)
		m_targetAngle = 360.0f + m_targetAngle;
	m_currentAngle = m_targetAngle;
}

void Ant::reflect_cell(int row, int col)
{
	// Vector from grid to ant
	glm::vec3 grid_to_ant = m_pos - g_demo.m_wallGrid.GetCoords(row, col);

	float abs_x = glm::abs(grid_to_ant.x);
	float abs_z = glm::abs(grid_to_ant.z);

	if (Near(abs_z, abs_x))
		reflect(glm::normalize(grid_to_ant));

	// Ant is to the left or right side
	else if (abs_z > abs_x)
	{
		if (grid_to_ant.z < 0.0f)
			reflect(glm::vec3(0.0f, 0.0f, -1.0f));
		else
			reflect(glm::vec3(0.0f, 0.0f, 1.0f));
	}

	// Ant is up or down
	else
	{
		if (grid_to_ant.x < 0.0f)
			reflect(glm::vec3(-1.0f, 0.0f, 0.0f));
		else
			reflect(glm::vec3(1.0f, 0.0f, 0.0f));
	}
}

#define ABS(a) (((a) < 0) ? -(a) : (a))
bool Ant::LineIntersect(float x1, float y1, float x2, float y2,
	float x3, float y3, float x4, float y4)
{
	float y4y3 = y4 - y3;
	float y1y3 = y1 - y3;
	float y2y1 = y2 - y1;
	float x4x3 = x4 - x3;
	float x2x1 = x2 - x1;
	float x1x3 = x1 - x3;
	float denom = y4y3 * x2x1 - x4x3 * y2y1;
	float numera = x4x3 * y1y3 - y4y3 * x1x3;
	float numerb = x2x1 * y1y3 - y2y1 * x1x3;

	const float eps = 0.0001f;
	if (ABS(numera) < eps && ABS(numerb) < eps && ABS(denom) < eps)
	{	//Lines coincident (on top of each other)
		return true;
	}

	if (ABS(denom) < eps)
	{	//Lines parallel
		return false;
	}

	float mua = numera / denom;
	float mub = numerb / denom;
	if (mua < 0 || mua > 1 || mub < 0 || mub > 1)
	{	//No intersection
		return false;
	}
	else
	{	//Intersection
		return true;
	}
}

bool Ant::path_is_clear(const glm::vec3& cellPos)
{
	int r0, c0, r1, c1;
	g_demo.m_wallGrid.GetRowColumn(m_pos, &r0, &c0);
	g_demo.m_wallGrid.GetRowColumn(cellPos, &r1, &c1);

	// Make a bounding box that encapsulates both cells
	int start_r = (std::min)(r0, r1);
	int start_c = (std::min)(c0, c1);
	int end_r = (std::max)(r0, r1);
	int end_c = (std::max)(c0, c1);

	// Wall border offsets (ccw - from bot-right)
	// true:  +offset
	// false: -offset
	bool offset_r[4] = { false,  true,  true,  false };
	bool offset_c[4] = { true,  true,  false,  false };

	// Half the size of a cell + epsilon
	float offset = WALL_CELL_HALF_WIDTH + 0.001f;

	// Go through all the cells inside the bv
	for (int r = start_r; r <= end_r; ++r)
	{
		for (int c = start_c; c <= end_c; ++c)
		{
			// Skip cells that are not walls
			if (g_demo.m_wallGrid.CellHasWall(r, c) == false)
				continue;

			// Precompute offsets
			glm::vec3 cellp = g_demo.m_wallGrid.GetCoords(r, c);
			float pos_off_r = cellp.x + offset;
			float neg_off_r = cellp.x - offset;
			float pos_off_c = cellp.z + offset;
			float neg_off_c = cellp.z - offset;

			// Check if the line intersects with any of
			// the 4 wall borders
			for (int i = 0; i < 4; ++i)
			{
				// Get the first point of the wall border
				float b_r0 = offset_r[i] ? pos_off_r : neg_off_r;
				float b_c0 = offset_c[i] ? pos_off_c : neg_off_c;

				// Get the index of the next point
				int next = (i + 1 < 4) ? i + 1 : 0;

				// Get the second point of the wall border
				float b_r1 = offset_r[next] ? pos_off_r : neg_off_r;
				float b_c1 = offset_c[next] ? pos_off_c : neg_off_c;
				

				// If lines intersect, the path is not clear
				if (LineIntersect((float)m_pos.x, (float)m_pos.z, (float)cellPos.x, (float)cellPos.z, b_r0, b_c0, b_r1, b_c1))
				{
					return false;
				}
			}
		}
	}

	// Path is clear
	return true;
}

glm::vec3 Ant::point_in_circunference()
{
	if (!g_demo.m_sampleAnts)
		m_currentAngle += (m_targetAngle - m_currentAngle) * 0.02f;
	else if (rand() % 3 == 0)
		m_targetAngle = (float)(rand() % 360);

	float z = cos(glm::radians(m_currentAngle)) * m_radius;
	float x = sin(glm::radians(m_currentAngle)) * m_radius;
	return m_pos + glm::vec3(x, 0.0f, z);
}
}