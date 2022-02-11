/**
* @file geometry.cpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "geometry.hpp"

namespace cs380 {

    // Input operator of the classification class
    std::istream& operator>>(std::istream& is, cs380::classification_t& c)
    {
        std::string classification;
        is >> classification;

        if (classification == "inside")
            c = cs380::classification_t::inside;
        else if (classification == "outside")
            c = cs380::classification_t::outside;
        else
            c = cs380::classification_t::overlapping;

        return is;
    }

    // Output operator of the classification class
    std::ostream& operator<<(std::ostream& os, const cs380::classification_t& c)
    {
        if (c == cs380::classification_t::inside)
            os << "inside";
        else if (c == cs380::classification_t::outside)
            os << "outside";
        else
            os << "overlapping";

        return os;
    }

    // Input operator for the segment class
    std::istream& operator>>(std::istream& is, segment& s)
    {
        is >> s.m_start >> s.m_end;
        return is;
    }

    // Ouput operator for the segment class
    std::ostream& operator<<(std::ostream& os, const segment& seg)
    {
        os << seg.m_start << " "  << seg.m_end;
        return os;
    }

    // Input operator for the plane class
    std::istream& operator>>(std::istream& is, plane& p)
    {
        is >> p.m_center >> p.m_normal;
        return is;
    }

    // Output operator for the plane class
    std::ostream& operator<<(std::ostream& os, const plane& p)
    {
        os << p.m_center << p.m_normal;
        return os;
    }

    // Input operator for the aabb class
    std::istream& operator>>(std::istream& is, aabb& cube)
    {
        is >> cube.m_min >> cube.m_max;
        return is;
    }

    // Output operator for the aabb class
	std::ostream& operator<<(std::ostream& os, const aabb& cube)
    {
        os << cube.m_min << " " << cube.m_max;
        return os;
    }
    
    // Input operator for the sphere class
    std::istream& operator>>(std::istream& is, sphere& s)
    {
        is >> s.m_center >> s.m_radius;
        return is;
    }

    // Output operator for the sphere class
    std::ostream& operator<<(std::ostream& os, const sphere& s)
    {
        os << s.m_center << " " << s.m_radius;
        return os;
    }

    // Input operator for the triangle class
    std::istream& operator>>(std::istream& is, triangle& t)
    {
        is >> t.m_points[0] >> t.m_points[1] >> t.m_points[2];
        return is;
    }

    // Output operator for the triangle class
    std::ostream& operator<<(std::ostream& os, const triangle& t)
    {
        os << t.m_points[0] << " " << t.m_points[1] << " " << t.m_points[2];
        return os;
    }

    // Input operator for the ray class
    std::istream& operator>>(std::istream& is, ray& r)
    {
        is >> r.m_origin >> r.m_direction;
        return is;
    }

    // Output operator for the ray class
    std::ostream& operator<<(std::ostream& os, const ray& r)
    {
        os << r.m_origin << " " << r.m_direction;
        return os;
    }

    // Input operator for the frustum class
    std::istream& operator>>(std::istream& is, frustum& f)
    {
        for (unsigned int i = 0; i < 6; ++i)
            is >> f.m_planes[i];
        return is;
    }

    // Output operator for the frustum class
    std::ostream& operator<<(std::ostream& os, const frustum& f)
    {
        for (unsigned int i = 0; i < 6; ++i)
            os << f.m_planes[i];
        return os;
    }

    // Returns true if all the elements in the given array have the same classification
	bool same_classification(classification_t* classifications, unsigned int size)
    {
        for (unsigned int i = 0; i < size; ++i)
        {
            // Found an elements with different classification
            if (classifications[0] != classifications[i])
                return false;
        }

        return true;
    }

    // Takes a classification array and counts how many inside/outside classifications there are
	void get_classification_numbers(classification_t* classifications, unsigned int size, unsigned int* outside, unsigned int* inside)
    {
        for (unsigned int i = 0; i < size; ++i)
        {
            if (inside && classifications[i] == classification_t::inside)
                *inside += 1;
            else if (outside && classifications[i] == classification_t::outside)
                *outside += 1;
        }
    }

    // Calculates the projection of the given point into the given plane
    const glm::vec3 closest_point_plane(const glm::vec3& point, const plane& plane)
    {
        return point - (glm::dot(point - plane.m_center, plane.m_normal) / glm::length2(plane.m_normal)) * plane.m_normal;
    }

    // Clamp a given value between min and max
    float clamp(float value, float min, float max)
    {
        if (value < min)
            return min;
        if (value > max)
            return max;
        return value;
    }

    // Return the closest segment that goes from one given segment to the other
    const segment closest_segment_segment(const segment& s1, const segment& s2)
    {
        // Implementation taken from the book

        // Same Segment
        if ((s1.m_start == s2.m_start && s1.m_end == s2.m_end) ||
            (s1.m_start == s2.m_end && s1.m_end == s2.m_start))
            return segment(s1.m_end, s1.m_end);

        // Get the direction of both segments
        glm::vec3 d1 = s1.m_end - s1.m_start;
        glm::vec3 d2 = s2.m_end - s2.m_start;

        glm::vec3 r = s1.m_start - s2.m_start;

        // Squared length of segment 1 and 2
        float a = glm::dot(d1, d1);
        float e = glm::dot(d2, d2);
        float f = glm::dot(d2, r);

        float s = 0.0f; // P = s1.m_start + s*d1 s[0,1]
        float t = 0.0f; // Q = s2.m_start + t*d2 t[0,1] 

        // Check if both segments are points (start == end)
        if (a <= cEpsilon && e <= cEpsilon)
        {
            return segment(s1.m_end, s2.m_end);
        }

        // Check if the first segment is a point
        if (a <= cEpsilon)
        {
            s = 0.0f; // P = s1.m_start + 0*d1

            // Get the parameter for the second segment
            t = clamp(f / e, 0.0f, 1.0f);
        }

        else
        {
            float c = glm::dot(d1, r);

            // Check if the second segment is a point
            if (e <= cEpsilon)
            {
                t = 0.0f; // Q = s2.m_start + 0*d2;

                // Get a parameter for the first segment
                s = clamp(-c / a, 0.0f, 1.0f);
            }

            // No segment is a point (General case)
            else
            {
                float b = glm::dot(d1, d2);
                float denom = a * e - b * b;

                if (denom != 0.0f)
                {
                    // Get a parameter for the first segment
                    s = clamp((b * f - c * e) / denom, 0.0f, 1.0f);
                }
                else
                    s = 0.0f;

                // Get the parameter for the second segment
                t = (b * s + f) / e;

                // Check if t is out of the expected range [0,1]
                if (t < 0.0f)
                {
                    // Clamp the parameter
                    t = 0.0f;
                    
                    // Recompute the parameter for the first segment
                    s = clamp(-c / a, 0.0f, 1.0f);
                }

                // Check if t out of the expected range [0,1]
                else if (t > 1.0f)
                {
                    // Clamp the parameter
                    t = 1.0f;

                    // Recompute the parameter for the first segment
                    s = clamp((b - c) / a, 0.0f, 1.0f);
                }
            }
        }
        
        // Compute the point in the first segment
        glm::vec3 P = s1.m_start + s * d1;

        // Compute the point in the second segment
        glm::vec3 Q = s2.m_start + t * d2;

        return segment(P, Q);
    }

    // Checks if the given point is intersecting with the given aabb
	bool intersection_point_aabb(const glm::vec3& point, const aabb& cube)
    {
        // Check for the x, y and z axis
        return (point.x >= cube.m_min.x && point.x <= cube.m_max.x &&
                point.y >= cube.m_min.y && point.y <= cube.m_max.y &&
                point.z >= cube.m_min.z && point.z <= cube.m_max.z);
    }

    // Checks if the given point is intersecting with the given sphere
    bool intersection_point_sphere(const glm::vec3& point, const sphere& s)
    {
        return glm::distance2(s.m_center, point) <= (s.m_radius * s.m_radius);
    }

    // Checks if the given point is inside/outside/overlapping the given plane
    classification_t classify_plane_point(const plane& plane, const glm::vec3& point, const float thickness)
    {
        float dot_product = glm::dot(plane.m_normal, point - plane.m_center);

        // Square distance from point to plane
        float dist2 = glm::length2((dot_product / glm::length2(plane.m_normal)) * plane.m_normal);

        // Point is overlapping the plane
        if (dist2 <= thickness * thickness)
            return classification_t::overlapping;

        // Point is inside the plane
        if (dot_product < 0)
            return classification_t::inside;
        
        // Point is outside the plane
        return classification_t::outside;
    }

    // Checks if the given triangle is inside/outside/overlapping the given plane
    classification_t classify_plane_triangle(const plane& plane, const triangle& triangle, const float thickness)
    {
        // Store the classification of each point in the triangle
        classification_t p_classif[3];

        // Check where is each point respect to the plane (inside/outside/overlapping)
        p_classif[0] = classify_plane_point(plane, triangle.m_points[0], thickness);
        p_classif[1] = classify_plane_point(plane, triangle.m_points[1], thickness);
        p_classif[2] = classify_plane_point(plane, triangle.m_points[2], thickness);

        // All points have the same classification
        if (p_classif[0] == p_classif[1] && p_classif[0] == p_classif[2])
            return p_classif[0];

        // Get the number of outside classifications
        unsigned int n_outside = 0;
        unsigned int n_inside = 0;
        get_classification_numbers(p_classif, 3, &n_outside, &n_inside);

        // No point inside the plane. (Outside and overlapping)
        if (n_inside == 0)
            return classification_t::outside;

        // No point outside the plane. (Inside and overlapping)
        if (n_outside == 0)
            return classification_t::inside;

        // At least one point inside and other outside
        return classification_t::overlapping;
    }

    // Checks if the given cube is inside/outside/overlapping the given plane
    classification_t classify_plane_aabb(const plane& plane, const aabb& cube, const float thickness)
    {
        // Implementation taken from the class' book:
        glm::vec3 center = (cube.m_max + cube.m_min) / 2.0f;

        // Positive extent
        glm::vec3 e = cube.m_max - center;

        // Compute the radius
        float r = e.x * std::abs(plane.m_normal.x) + e.y * std::abs(plane.m_normal.y) + e.z * std::abs(plane.m_normal.z);

        // Distance from cube's center to the plane
        float dist = glm::dot(plane.m_normal, center) - glm::dot(plane.m_normal, plane.m_center);

        // Check if the distance from cube to plane is smaller or equal than the radius
        if (std::abs(dist) <= r)
            return classification_t::overlapping;

        // Inside the plane (angle > pi/2)
        if (glm::dot(plane.m_normal, center - plane.m_center) < 0)
            return classification_t::inside;
        
        // Outside the plane
        return classification_t::outside;
    }

    // Checks if the given sphere is inside/outside/overlapping the given plane
    classification_t classify_plane_sphere(const plane& plane, const sphere& sphere, const float thickness)
    {
        float dot_product = glm::dot(plane.m_normal, sphere.m_center - plane.m_center);

        // Square distance from sphere's center to plane
        float dist2 = glm::length2((dot_product / glm::length2(plane.m_normal)) * plane.m_normal);

        // Sphere is overlapping the plane
        if (dist2 <= (sphere.m_radius + thickness) * (sphere.m_radius + thickness))
            return classification_t::overlapping;

        // Sphere is inside the plane
        if (dot_product < 0)
            return classification_t::inside;
        
        // Sphere is outside the plane
        return classification_t::outside;
    }

    // Checks if two given cubes are intersecting
	bool intersection_aabb_aabb(const aabb& cube1, const aabb& cube2)
    {
        glm::vec3 pos1 = (cube1.m_max + cube1.m_min) / 2.0f;
        glm::vec3 pos2 = (cube2.m_max + cube2.m_min) / 2.0f;
        
        glm::vec3 scale1 = (cube1.m_max - cube1.m_min);
        glm::vec3 scale2 = (cube2.m_max - cube2.m_min);

        // Get the vector that goes from one cube to the other
        glm::vec3 dist_vec = pos2 - pos1;

        // Get the absolute values of the distance in each axis
        dist_vec.x = dist_vec.x < 0 ? -dist_vec.x : dist_vec.x;
        dist_vec.y = dist_vec.y < 0 ? -dist_vec.y : dist_vec.y;
        dist_vec.z = dist_vec.z < 0 ? -dist_vec.z : dist_vec.z;

        // Check if the cubes are not interecting in the x, y or z axis
        if (dist_vec.x > scale1.x / 2.0f + scale2.x / 2.0f)
            return false;
        if (dist_vec.y > scale1.y / 2.0f + scale2.y / 2.0f)
            return false;
        if (dist_vec.z > scale1.z / 2.0f + scale2.z / 2.0f)
            return false;

        // Cubes are intersecting in all axis
        return true;
    }

    // Checks if two given spheres are intersecting
	bool intersection_sphere_sphere(const sphere& sphere1, const sphere& sphere2)
    {
        // Get the squared distance between the two spheres
        float dist2 = glm::distance2(sphere1.m_center, sphere2.m_center);

        // Check if the spheres are intersecting
        return dist2 <= (sphere1.m_radius + sphere2.m_radius) * (sphere1.m_radius + sphere2.m_radius);
    }

    // Checks the intersection between ray and plane. Returns the t parameter
    float intersection_ray_plane(const ray& ray, const plane& plane)
    {
        // Calculate the d of the plane's formula (ax + by + cz = d)
        float d = glm::dot(plane.m_normal, plane.m_center);

        // Check if the ray and the plane are parallel
        float ray_dot_normal = glm::dot(plane.m_normal, ray.m_direction);
        if (ray_dot_normal >= -cEpsilon && ray_dot_normal <= cEpsilon)
            return -1.0f;

        // Calculate the parameter t
        float t = (d - glm::dot(plane.m_normal, ray.m_origin)) / ray_dot_normal;

        // For intersection to exist t's range: [0, infinity)
        if (t >= 0.0f) 
            return t;
        
        return -1.0f;
    }

    // Checks the intersection between ray and aabb. Returns t parameter
	float intersection_ray_aabb(const ray& ray, const aabb& cube)
    {
        // Implementation taken from the book
        float t_min = 0.0f;
        float t_max = FLT_MAX;

        // Go through all slabs (x,y,z)
        for (unsigned int i = 0; i < 3; ++i)
        {
            // Check if the ray is parallel to the slab
            if (std::abs(ray.m_direction[i]) < cEpsilon)
            {
                // Check if the origin of the ray is not within the slab
                if (ray.m_origin[i] < cube.m_min[i] || ray.m_origin[i] > cube.m_max[i])
                    return -1.0f;
            }

            else
            {
                // Compute the parameter t for intersection with both planes of the slab
                float ood = 1.0f / ray.m_direction[i];
                float t1 = (cube.m_min[i] - ray.m_origin[i]) * ood;
                float t2 = (cube.m_max[i] - ray.m_origin[i]) * ood;

                // t1 should be the intersection with the near plane
                // So, the lowest value
                if (t1 > t2)
                {
                    float temp = t1;
                    t1 = t2;
                    t2 = temp;
                }

                // Update the last entry and first exit
                if (t1 > t_min)
                    t_min = t1;
                if (t2 < t_max)
                    t_max = t2;

                // No intersection with the slab
                if (t_min > t_max)
                    return -1.0f;
            }
        }

        // There is no entry
        if (t_min > 0.0f)
            return t_min;

        // Ray intersects the cube
        return 0.0f;
    }

    // Checks the intersection between ray and triangle. Returns t parameter
	float intersection_ray_triangle(const ray& ray, const triangle& triangle)
    {
        //get the vector from the triangles
		glm::vec3 v = triangle.m_points[1] - triangle.m_points[0];
		glm::vec3 w = triangle.m_points[2] - triangle.m_points[0];

		//get the normal vector of the triangle
		glm::vec3 tNormal = glm::cross(v, w);

		//create a plane to perform the ray plane test
		plane tPlane;
		tPlane.m_center = triangle.m_points[0]; tPlane.m_normal = tNormal;

		//perform the ray plane test
		float t = intersection_ray_plane(ray, tPlane);

		if (t == -1.f) return -1.f;

		//compute intersection position
		glm::vec3 p = ray.m_origin + t * ray.m_direction;

		glm::vec3 u = p - triangle.m_points[0];

		//barycentric coordinates
		//create 2x2 matrices
		glm::mat2 s1 = glm::mat2(glm::dot(u, v), glm::dot(w, v), glm::dot(u, w), glm::dot(w, w));
		glm::mat2 s2 = glm::mat2(glm::dot(v, v), glm::dot(w, v), glm::dot(v, w), glm::dot(w, w));

		//get the s value by divinding the determinants
		float bary_s = glm::determinant(s1) / glm::determinant(s2);

		glm::mat2 t1 = glm::mat2(glm::dot(v, v), glm::dot(u, v), glm::dot(v, w), glm::dot(u, w));
		glm::mat2 t2 = glm::mat2(glm::dot(v, v), glm::dot(w, v), glm::dot(v, w), glm::dot(w, w));

		//get the s value by divinding the determinants
		float bary_t = glm::determinant(t1) / glm::determinant(t2);

		// perform checks
		if (bary_s < 0.f || bary_s > 1.f) return -1.f;
		if (bary_t < 0.f || bary_t > 1.f) return -1.f;
		if (1 - bary_s - bary_t < 0.f || 1 - bary_s - bary_t > 1.f) return -1.f;

		return t;
    }

    // Checks the intersection between ray and sphere. Returns t parameter
	float intersection_ray_sphere(const ray& ray, const sphere& sphere)
    {
        // Follow the slides formula
        float a = glm::length2(ray.m_direction);

        // Sanity check. Avoid dividing by 0
        if (a >= -cEpsilon && a <= cEpsilon)
            return -1.0f;

        float b = 2.0f * (glm::dot(ray.m_direction, ray.m_origin - sphere.m_center));
        float c = glm::distance2(ray.m_origin, sphere.m_center) - sphere.m_radius * sphere.m_radius;
        
        // Store both t parameters. The ray might intersect the sphere twice.
        // When entering and exiting the sphere.
        float t1, t2;
        float b2_minus_4ac = b * b - 4.0f * a * c;

        // There is no solution. No intersection
        if (b2_minus_4ac < 0)
            return -1.0f;

        float sq_root = sqrt(b2_minus_4ac);
        float denominator = 2.0f * a;

        t1 = (-b + sq_root) / denominator;
        t2 = (-b - sq_root) / denominator;

        // Ray does not intersect
        if (t1 < 0.0f && t2 < 0.0f)
            return -1.0f;

        // Two intersections found
        if (t1 >= 0.0f && t2 >= 0.0f)
        {
            // Return the smallest t
            if (t1 < t2)
                return t1;
            
            return t2;
        }

        // The ray is inside the sphere
        return 0.0f;
    }

    // Checks if the given sphere is inside/outside/overlapping the given frustum
	classification_t classify_frustum_sphere_naive(const frustum& frustum, const sphere& sphere)
    {
        // Store the classification of the sphere with each plane
        classification_t s_classif[6];

        for (unsigned int i = 0; i < 6; ++i)
            s_classif[i] = classify_plane_sphere(frustum.m_planes[i], sphere, cEpsilon);

        // The sphere has the same classification with all planes
        if (same_classification(s_classif, 6))
            return s_classif[0];
        
        // Get the number of outside classifications
        unsigned int n_outside = 0;
        get_classification_numbers(s_classif, 6, &n_outside);

        // The sphere is inside some planes and overlapping on others
        if (n_outside == 0)
            return classification_t::overlapping;

        // The sphere is outside the frustum
        return classification_t::outside;
    }

    // Checks if the given cube is inside/outside/overlapping the given frustum
	classification_t classify_frustum_aabb_naive(const frustum& frustum, const aabb& cube)
    {
        // Store the classification of the cube with each plane
        classification_t s_classif[6];

        for (unsigned int i = 0; i < 6; ++i)
            s_classif[i] = classify_plane_aabb(frustum.m_planes[i], cube, cEpsilon);

        // The cube has the same classification with all planes
        if (same_classification(s_classif, 6))
            return s_classif[0];
        
        // Get the number of outside classifications
        unsigned int n_outside = 0;
        get_classification_numbers(s_classif, 6, &n_outside);

        // The cube is inside some planes and overlapping on others
        if (n_outside == 0)
            return classification_t::overlapping;

        // The cube is outside the frustum
        return classification_t::outside;
    }

    segment::segment()
        : m_start(0.0f)
        , m_end(0.0f)
    {}

    // Store the 2 points of the segment
    segment::segment(const glm::vec3& s, const glm::vec3& e)
        : m_start(s)
        , m_end(e)
    {}

    glm::vec3& segment::operator[](const int index)
    {
        // Avoid accessing out of bounds memory
        assert(index < 2 && index >= 0);

        if (index == 0)
            return m_start;
        else
            return m_end;
    }

    const glm::vec3& segment::operator[](const int index) const
    {
        // Avoid accessing out of bounds memory
        assert(index < 2 && index >= 0);

        if (index == 0)
            return m_start;
        else
            return m_end;
    }

    triangle::triangle()
    {
        m_points[0] = glm::vec3(0.0f);
        m_points[1] = glm::vec3(0.0f);
        m_points[2] = glm::vec3(0.0f);
    }

    // Store the 3 points of the triangle
    triangle::triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
    {
        m_points[0] = p1;
        m_points[1] = p2;
        m_points[2] = p3;
    }

    glm::vec3 triangle::centroid() const
    {
        return (m_points[0] + m_points[1] + m_points[2]) / 3.0f;
    }

    aabb triangle::compute_bv() const
    {
        // Initialize the bv with the first point as min and max
        aabb bv(m_points[0], m_points[0]);

        // Update the bv
        for (int vertex = 1; vertex < 3; ++vertex)
		{
			for (int axis = 0; axis < 3; ++axis)
			{
				// Update the bv of the root node
				if (m_points[vertex][axis] < bv.m_min[axis])
					bv.m_min[axis] = m_points[vertex][axis];	
				else if (m_points[vertex][axis] > bv.m_max[axis])
					bv.m_max[axis] = m_points[vertex][axis];
			}
		}

        return bv;
    }

    glm::vec3& triangle::operator[](int index)
    {
        assert(index >= 0 && index < 3);
        return m_points[index];
    }

    const glm::vec3& triangle::operator[](int index) const
    {
        assert(index >= 0 && index < 3);
        return m_points[index];
    }

    glm::vec3 triangle::normal() const
    {
        return glm::normalize(glm::cross(m_points[1] - m_points[0], m_points[2] - m_points[0]));
    }

    plane::plane()
        : m_center(0.0f)
        , m_normal(0.0f)
    {}

    plane::plane(const glm::vec3 pos, const glm::vec3 norm)
        : m_center(pos)
        , m_normal(norm)
    {}

    aabb::aabb()
        : m_min(0.0f)
        , m_max(0.0f)
    {}

    // Compute the position, rotation and scale of the object
    aabb::aabb(const glm::vec3& min, const glm::vec3& max)
        : m_min(min)
        , m_max(max)
    {
    }

    glm::mat4 aabb::model_to_world() const
    {
        // Compute the position of the cube
        glm::vec3 pos = (m_min + m_max) / 2.0f;

        // Compute the scale of the cube
        glm::vec3 scale = m_max - m_min;

        // Translation matrix
        glm::mat4 TRS(1.0f);
        TRS = glm::translate(TRS, pos);

        // Scale matrix
        TRS = glm::scale(TRS, scale);

        return TRS;
    }

    sphere::sphere()
        : m_radius(0.0f)
        , m_center(0.0f)
    {}

    // Compute the position, rotation and scale of the object
    sphere::sphere(const glm::vec3& center, const float radius)
        : m_radius(radius)
        , m_center(center)
    {}

    glm::mat4 sphere::model_to_world() const
    {
        // Translation matrix
        glm::mat4 TRS(1.0f);
        TRS = glm::translate(TRS, m_center);

        // Scale matrix
        TRS = glm::scale(TRS, glm::vec3(m_radius));

        return TRS;
    }

    ray::ray()
        : m_origin(0.0f)
        , m_direction(0.0f)
    {}

    frustum::frustum()
    {}
}