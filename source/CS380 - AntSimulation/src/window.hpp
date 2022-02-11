/**
* @file window.hpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once
#include "math.hpp"

namespace cs380{
	class window {
	private:
		GLFWwindow* m_window = {};
		glm::ivec2 m_size = {};
		bool fullscreen = false;

	public:
		~window() {};
		bool create(int w, int h, const char* window_name, bool hidden);
		bool update();
		void destroy();
		void enable_vsync();
		void toggle_fullscreen();

		[[nodiscard]] glm::ivec2 size() const noexcept {return m_size;}
		[[nodiscard]] GLFWwindow* handle() const noexcept {return m_window;}
	};
}