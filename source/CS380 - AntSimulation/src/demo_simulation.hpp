/**
* @file demo_gjk.hpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/12/10
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once
#include "fwd.hpp"
#include "math.hpp"
#include "geometry.hpp"
#include "global.h"
#include "Ant.h"
#include "FoodGrid.h"
#include "PheromoneGrid.h"
#include "WallGrid.h"

const int MAX_ANTS = 1000;
const float ANT_SAMPLE_TIME = 2.0f;

const float PHEROMONE_DROP_TIME = 0.2f;
const float PHEROMONE_LOOK_TIME = 0.2f;

class renderer;
class camera;
class window;
namespace cs380 {
	class demo {
	public:
		static demo& instance()
		{
			static demo inst;
			return inst;
		}

		void create();
		void destroy();
		bool update();

		WallGrid m_wallGrid;
		FoodGrid m_foodGrid;		   // 2d uniform grid for food
		PheromoneGrid m_pheromoneGrid; // 2d uniform grid for pheromones
		glm::vec3 m_nestPos = glm::vec3(FLOOR_SCALE_X / 2.0f, 0.0f, FLOOR_SCALE_Z / 2.0f);

		// ANT SIMULATION CONFIGURABLE PARAMS
		int m_pheromoneExponent = 1;		// Can take any positive value
		int m_distanceExponent = 1;			// Can take any positive value
		float m_evaporationRate = 0.001f;	// In range [0,1]

		float m_pheromoneLookTimer = 0.0f;
		float m_pheromoneLookTime = PHEROMONE_LOOK_TIME;
		bool m_lookForPheromones = true;

		float m_antSampleTimer = 0.0f;
		float m_antSampleTime = ANT_SAMPLE_TIME;
		bool m_sampleAnts = true;

		float m_dropPheromoneTimer = 0.0f;
		float m_dropPheromoneTime = PHEROMONE_DROP_TIME;
		bool m_dropPheromone = true;

		bool m_nestPlaced = false;

		bool m_isSimulationPaused = false;

		float m_simulationSpeed = 1.0f;

		int m_antFovRadius = 15;

		int m_antNumber = 1000;

		int m_pheromonesToDrop = 1;
		int m_maxPheromoneIntensity = (int)(m_pheromonesToDrop * m_antNumber * m_maxPheromoneFactor);
		float m_maxPheromoneFactor = 1.0f;
		int m_pheromonesPerAnt = 100;

		float dt() const;

	private:
		// For camera update
		float m_last_frame_time = { 0.0f };
		glm::vec2 m_cursor_pos = { 0.0f, 0.0f };

		// Receive input
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		// Imgui logic
		void show_gui();

		// Render
		void draw();

		void show_origin() const;

		void compute_mouse_pos();

		// Camera movement
		void input();

		// Spherical camera
		float alpha = 0.0f; // Angle on yz plane
		float beta = 0.0f; // Angle on xz plane
		float radius = 0.0f;

		renderer* r = nullptr;
		camera* cam = nullptr;
		window* win = nullptr;

		float current_frame_time = 0.0f;
		float prev_frame_time = 0.0f;
		glm::vec3 mouse_pos = glm::vec3(0.0f);

		Ant m_ants[MAX_ANTS];	// Holds the ants available

		// Draws the nest of the ant colony
		void draw_nest(renderer* r);
		void reset_ants();
		void set_nest_pos(const glm::vec3& pos);

		void simulation_logic();

		void place_nest_logic();
		void place_food_logic();
		void place_walls_logic();

		int curr_lclick = 0;
		int prev_lclick = 0;
		bool lclick_press = false; // User pressed the left mouse button
		bool lclick_down = false;  // User is holding the left mouse button
		int curr_rclick = 0;
		int prev_rclick = 0;
		bool rclick_press = false; // User pressed the left mouse button
		bool rclick_down = false;  // User is holding the left mouse button

		void drop_food_pheromone(const glm::vec3& pos, float value);
		void drop_nest_pheromone(const glm::vec3& pos, float value);

		glm::vec3 floor_pos = glm::vec3(FLOOR_HALF_SCALE_X, 0.0f, FLOOR_HALF_SCALE_Z);
		glm::vec3 floor_scale = glm::vec3(FLOOR_HALF_SCALE_X, FLOOR_HALF_SCALE_Z, 1.0f);
		glm::vec3 floor_rot = glm::vec3(90.0f, 0.0f, 0.0f);
		glm::vec3 ant_scale = glm::vec3(ANT_SCALE, ANT_SCALE, ANT_SCALE * 3.0f);

		// 0 for nest, 1 for food, 2 for walls
		int m_editMode = 0;
	};
}