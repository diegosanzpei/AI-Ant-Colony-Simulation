/**
* @file geometry.hpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once
#include "math.hpp"
#include <iostream>

namespace cs380 {
	struct segment
	{
		segment();
		segment(const glm::vec3& s, const glm::vec3& e);

		// Subscript operator
		glm::vec3& operator[](int index);
		const glm::vec3& operator[](const int index) const;

		// Input/Output operators
		friend std::istream& operator>>(std::istream& is, segment& s);
		friend std::ostream& operator<<(std::ostream& os, const segment& s);

		glm::vec3 m_start;
		glm::vec3 m_end;
	};

	struct plane
	{
		plane();

		plane(const glm::vec3 pos, const glm::vec3 norm);

		// Input/Output operators
		friend std::istream& operator>>(std::istream& is, plane& p);
		friend std::ostream& operator<<(std::ostream& os, const plane& p);

		glm::vec3 m_center;
		glm::vec3 m_normal;
	};

	struct aabb
	{
		aabb();
		aabb(const glm::vec3& min, const glm::vec3& max);

		glm::mat4 model_to_world() const;

		// Input/Output operators
		friend std::istream& operator>>(std::istream& is, aabb& cube);
		friend std::ostream& operator<<(std::ostream& os, const aabb& cube);

		glm::vec3 m_min;
		glm::vec3 m_max;
	};

	struct triangle
	{
		triangle();
		triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

		// Computes the triangle's centroid
		glm::vec3 centroid() const;

		aabb compute_bv() const; 

		glm::vec3& operator[](int index);
		const glm::vec3& operator[](int index) const;

		glm::vec3 normal() const;

		// Input/Output operators
		friend std::istream& operator>>(std::istream& is, triangle& t);
		friend std::ostream& operator<<(std::ostream& os, const triangle& t);

		glm::vec3 m_points[3];
	};

	struct sphere
	{
		sphere();
		sphere(const glm::vec3& center, const float radius);

		glm::mat4 model_to_world() const;

		// Input/Output operators
		friend std::istream& operator>>(std::istream& is, sphere& s);
		friend std::ostream& operator<<(std::ostream& os, const sphere& s);
		
		glm::vec3 m_center;
		float m_radius;
	};

	struct ray
	{
		ray();

		// Input/Output operators
		friend std::istream& operator>>(std::istream& is, ray& r);
		friend std::ostream& operator<<(std::ostream& os, const ray& r);

		glm::vec3 m_origin;
		glm::vec3 m_direction;
	};

	struct frustum
	{
		frustum();

		// Input/Output operators
		friend std::istream& operator>>(std::istream& is, frustum& f);
		friend std::ostream& operator<<(std::ostream& os, const frustum& f);

		plane m_planes[6];
	};

	enum class classification_t { inside, outside, overlapping };

	// Input/Output operators of the enum class
	std::istream& operator>>(std::istream& is, cs380::classification_t& c);
	std::ostream& operator<<(std::ostream& os, const cs380::classification_t& c);

	// Calculates the projection of the given point into the given plane
	const glm::vec3 closest_point_plane(const glm::vec3& point, const plane& plane);

	// Clamp a given value between min and max
	float clamp(float value, float min, float max);

	// Return the closest segment that goes from one given segment to the other
	const segment closest_segment_segment(const segment& s1, const segment& s2);

	// Checks if the given point is intersecting with the given aabb
	bool intersection_point_aabb(const glm::vec3& point, const aabb& cube);

	// Checks if the given point is intersecting with the given sphere
	bool intersection_point_sphere(const glm::vec3& point, const sphere& s);

	// Checks if the given point is inside/outside/overlapping the given plane
	classification_t classify_plane_point(const plane& plane, const glm::vec3& point, const float thickness);

	// Checks if the given triangle is inside/outside/overlapping the given plane
	classification_t classify_plane_triangle(const plane& plane, const triangle& triangle, const float thickness);

	// Checks if the given cube is inside/outside/overlapping the given plane
	classification_t classify_plane_aabb(const plane& plane, const aabb& cube, const float thickness);
	
	// Checks if the given sphere is inside/outside/overlapping the given plane
	classification_t classify_plane_sphere(const plane& plane, const sphere& sphere, const float thickness);

	// Checks if two given cubes are intersecting
	bool intersection_aabb_aabb(const aabb& cube1, const aabb& cube2);

	// Checks if two given spheres are intersecting
	bool intersection_sphere_sphere(const sphere& sphere1, const sphere& sphere2);

	// Checks the intersection between ray and plane. Returns the t parameter
	float intersection_ray_plane(const ray& ray, const plane& plane);

	// Checks the intersection between ray and aabb. Returns t parameter
	float intersection_ray_aabb(const ray& ray, const aabb& cube);

	// Checks the intersection between ray and triangle. Returns t parameter
	float intersection_ray_triangle(const ray& ray, const triangle& triangle);

	// Checks the intersection between ray and sphere. Returns t parameter
	float intersection_ray_sphere(const ray& ray, const sphere& sphere);

	// Checks if the given sphere is inside/outside/overlapping the given frustum
	classification_t classify_frustum_sphere_naive(const frustum& frustum, const sphere& sphere);

	// Checks if the given cube is inside/outside/overlapping the given frustum
	classification_t classify_frustum_aabb_naive(const frustum& frustum, const aabb& cube);

	// Returns true if all the elements in the given array have the same classification
	bool same_classification(classification_t* classifications, unsigned int size);

	// Takes a classification array and counts how many inside/outside classifications there are
	void get_classification_numbers(classification_t* classifications, unsigned int size, unsigned int* outside, unsigned int* inside = nullptr);
}