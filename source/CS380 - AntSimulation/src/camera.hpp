/**
* @file debug.hpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once

#include "math.hpp"
namespace cs380 {
    class camera
    {
    public:
    camera();

    // Sets the position of the camera
    void set_position(const glm::vec3& p);

    // Sets the view vector of the camera
    void set_target(const glm::vec3& t);

    // Sets the projection matrix (from camera model space to projection space)
    void set_projection(const float fov, const glm::vec2& size, const float near, const float far);

    // Update the matrices
    void update();

    // Returns a reference to the camera's world to projection matrix
    const glm::mat4& get_world_to_projection();

    glm::vec3& position();
    glm::vec3& view_vec();
    const glm::vec3& right_vec() const;
    const glm::vec3& up_vec() const;

    glm::mat4 m_projection;
    glm::vec3 m_pos;
    glm::vec3 m_view_vec;
    glm::vec3 m_right_vec;
    glm::vec3 m_up_vec;

    glm::mat4 m_world_to_local;
    glm::mat4 m_world_to_projection;
    };
}