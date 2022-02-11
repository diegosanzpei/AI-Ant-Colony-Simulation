#pragma once
#include <algorithm>
static const float DEFAULT_DECAY = 0.05f;
static const float DEFAULT_GROWING = 0.150f;
static const float DEFAULT_UPDATEFREQUENCY = 0.066f;
static const float DEFAULT_FOVANGLE = 180.0f;
static const float DEFAULT_FOVCLOSEDISTANCE = 2.0f;

static bool Near(float x, float y) { return std::abs(x - y) < 1e-5f; }

static float rand_float_in_range(float low, float high)
{
	return low + ((float)rand() / (float)RAND_MAX) * (high - low);
}

static float EaseInExpo(float tn)
{
	return (float)(tn == 0 ? 0 : (float)pow(2, 10 * tn - 10));
}

// Constant added by me
static const float SQRT2 = (float)sqrt(2.0f);
static const float PI = 3.141592f;

// FOOR
const float FLOOR_SCALE_X = 4.0f;
const float FLOOR_SCALE_Z = FLOOR_SCALE_X * 16.0f / 9.0f;
const float FLOOR_HALF_SCALE_X = FLOOR_SCALE_X / 2.0f;
const float FLOOR_HALF_SCALE_Z = FLOOR_SCALE_Z / 2.0f;

// FOOD GRID
const int FOOD_GRID_COLS = 200;
const float FOOD_CELL_WIDTH = FLOOR_SCALE_Z / FOOD_GRID_COLS;
const float FOOD_CELL_HALF_WIDTH = FOOD_CELL_WIDTH / 2.0f;
const int FOOD_GRID_ROWS = (int)(FLOOR_SCALE_X / FOOD_CELL_WIDTH);

// WALL GRID
const int WALL_GRID_COLS = 50;
const float WALL_CELL_WIDTH = FLOOR_SCALE_Z / WALL_GRID_COLS;
const float WALL_CELL_HALF_WIDTH = WALL_CELL_WIDTH / 2.0f;
const int WALL_GRID_ROWS = (int)(FLOOR_SCALE_X / WALL_CELL_WIDTH);

// PHEROMONE GRID
const int PHEROMONE_GRID_COLS = 225;
const float PHEROMONE_CELL_WIDTH = FLOOR_SCALE_Z / PHEROMONE_GRID_COLS;
const float PHEROMONE_HALF_CELL_WIDTH = PHEROMONE_CELL_WIDTH / 2.0f;
const int PHEROMONE_GRID_ROWS = (int)(FLOOR_SCALE_X / PHEROMONE_CELL_WIDTH);
const int MAX_PHEROMONE_INTENSITY = 500;

const int FOOD_RADIUS = 3;
const int NEST_RADIUS = 3;
const int NEST_RADIUS_SQ = NEST_RADIUS * NEST_RADIUS;

const int MAX_FOOD_PER_CELL = 25;

const float MAX_DIST_PHEROMONE = FLOOR_SCALE_X / 2.5f;

const float ANTS_Y = 0.025f;
const float NEST_Y = 0.02f;
const float FOOD_Y = 0.015f;
const float PHEROMONES_Y = 0.01f;

const float ANT_SCALE = 0.015f;
const float WORLD_EDGE = 4.0f * ANT_SCALE;

// Neighbour nodes offsets
const int child_r_offset[8]{ -1,  0,  1,  1,  1,  0, -1, -1 };
const int child_c_offset[8]{ 1,  1,  1,  0, -1,  -1, -1, 0 };

#define g_demo cs380::demo::instance()