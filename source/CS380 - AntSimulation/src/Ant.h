#pragma once
#include "global.h"
#include "math.hpp"

namespace cs380 {
class Ant
{
public:
	Ant();

	void update();

	void integrate();

	void add_force(const glm::vec3& force);

	glm::vec3 pos() const;
	glm::vec3 forward() const;
	float speed() const;
	float radius() const;
	bool has_food() const;

	void reset(const glm::vec3& pos = glm::vec3(FLOOR_SCALE_X, 0.0f, FLOOR_SCALE_Z));

	int m_pheromoneCtr;

private:

	glm::vec3 m_pos;
	glm::vec3 m_forward;
	float m_speed;
	glm::vec3 m_force;
	glm::vec3 m_desiredDir;

	// For wandering
	float m_radius = 0.1f;
	float m_currentAngle = 0.0f;
	float m_targetAngle = 0.0f;

	// m_hasFood is false: Wandering for red pheromones or home
	// m_hasFood is true: Wandering for greed pheromones or food
	bool m_hasFood = false;
	bool m_isFollowingPheromone = false;


	void world_edges();
	void avoid_walls();
	void leave_food();
	void get_food();
	void wander();

	bool look_for_food(glm::vec3& dir);
	bool look_for_food_pheromones(glm::vec3& dir);

	bool look_for_nest(glm::vec3& dir);
	bool look_for_nest_pheromones(glm::vec3& dir);

	void reflect(const glm::vec3& normal);

	void reflect_cell(int row, int col);

	bool path_is_clear(const glm::vec3& cellPos);

	bool LineIntersect(float x1, float y1, float x2, float y2,
		float x3, float y3, float x4, float y4);

	glm::vec3 point_in_circunference();
};
}