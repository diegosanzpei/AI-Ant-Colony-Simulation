/**
* @file debug.hpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once
#include "math.hpp"
#include "geometry.hpp"

// IO
namespace glm{
    std::istream& operator>>(std::istream& is, vec2& v);
    std::ostream& operator<<(std::ostream& os, vec2 const& p);
	std::istream& operator>>(std::istream& is, vec3& v);
	std::ostream& operator<<(std::ostream& os, vec3 const& p);
}

namespace cs380{
	void debug_set_data(const void* data, unsigned int size, unsigned int stride);
	void debug_set_uniforms(const glm::mat4& model, const glm::vec4& color);
	void debug_unbind();
	
	// Debug drawing
	void debug_draw_point(glm::vec3 pt, glm::vec4 color);
	void debug_draw_points(const mesh* mesh, glm::mat4 const& m2w, glm::vec4 color);
	void debug_draw_segment(segment const& s, glm::vec4 color);
	void debug_draw_segments(const mesh* mesh, glm::mat4 const& m2w, glm::vec4 color);
	void debug_draw_triangle(triangle const& t, glm::vec4 color);
	void debug_draw_plane(glm::vec3 pt, glm::vec3 n, glm::vec4 color, float scale = 1.0f);
	void debug_draw_aabb(aabb const& a, glm::vec4 color, bool wire = false);
	void debug_draw_sphere(sphere const& s, glm::vec4 color);
    void debug_draw_sphere_discs(sphere const& s, glm::vec4 color);
    void debug_draw_frustum(frustum const& s, glm::vec4 color);
	void debug_draw_fancy(const mesh* mesh, glm::mat4 const& m2w, glm::vec4 color);
	void debug_draw_plain_color(const mesh*, glm::mat4 const& m2w, glm::vec4 const& color);
}