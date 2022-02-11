/**
* @file camera.cpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "camera.hpp"

namespace cs380 {

    camera::camera()
        : m_pos(glm::vec3(0.0f, 0.0f, 0.0f))
        , m_view_vec(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_world_to_local(0.0f)
        , m_world_to_projection(0.0f)
        , m_projection(0.0f)
        , m_right_vec(0.0f)
        , m_up_vec(0.0f)
    {}

    void camera::set_position(const glm::vec3& p)
    {
        m_pos = p;
    }

    void camera::set_target(const glm::vec3& t)
    {
        m_view_vec = glm::normalize(t - m_pos);
    }

    void camera::set_projection(const float fov, const glm::vec2& size, const float near, const float far)
    {
        m_projection = glm::perspective(glm::radians(fov), size.x/size.y, near, far);
        update();
    }

    void camera::update()
    {
        // Get the right and up vector of the camera
        m_right_vec = glm::cross(m_view_vec, glm::vec3(0.0f, 1.0f, 0.0f));
        m_up_vec = glm::cross(m_right_vec, m_view_vec);

        // Matrix to transform the camera from world to local space
        m_world_to_local = glm::lookAt(m_pos, m_pos + glm::normalize(m_view_vec), glm::normalize(m_up_vec));

        // Matrix to transform the camera from world to projection space
        m_world_to_projection = m_projection * m_world_to_local;
    }

    const glm::mat4& camera::get_world_to_projection()
    {
        return m_world_to_projection;
    }

    glm::vec3& camera::position()
    {
        return m_pos;
    }
    
    glm::vec3& camera::view_vec()
    {
        return m_view_vec;
    }

    const glm::vec3& camera::right_vec() const
    {
        return m_right_vec;
    }

    const glm::vec3& camera::up_vec() const
    {
        return m_up_vec;
    }
}