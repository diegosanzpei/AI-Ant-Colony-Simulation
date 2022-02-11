/**
 * @file opengl.hpp
 * @author Eder Beldad (eder.b1@digipen.edu)
 * @date 2020-06-01
 * 
 * @copyright Copyright (c) 2020
 * @brief Include this file so you don't include opengl every time. Only in source files
 */
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace cs350{

	void enable_gl_callbacks();
	std::vector<glm::vec<4, unsigned char>> take_screenshoot(unsigned width, unsigned height);
	std::vector<glm::vec<4, unsigned char>> save_screenshoot(unsigned width, unsigned height, std::string const& filename);
}