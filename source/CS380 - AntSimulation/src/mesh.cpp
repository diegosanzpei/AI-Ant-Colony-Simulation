/**
* @file mesh.cpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "mesh.hpp"
#include "opengl.hpp"
#include "renderer.hpp"

namespace cs380 {
    mesh::mesh()
        : m_vao(0)
        , m_vbo_pos(0)
        , m_ebo_pos(0)
        , n_elements(0)
        , n_points(0)
    {}
    
    // Initialize the mesh with a mesh_data:
    // Create buffers
    // Send data to gpu
    // Save opengl handles
    void mesh::create(const mesh_data& data)
    {
        // Generate vao, vbos and ebos
        glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo_pos);
        glGenBuffers(1, &m_ebo_pos);

        // Bind vao
        glBindVertexArray(m_vao);

        // Send the position of the vertices to the GPU
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_pos);
        glBufferData(GL_ARRAY_BUFFER, data.positions.size() * sizeof(glm::vec3), data.positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
        glEnableVertexAttribArray(0);

        // Send the indices of the positions
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_pos);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.pos_indices.size() * sizeof(unsigned int), data.pos_indices.data(), GL_STATIC_DRAW);

        // Unbind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Get the number of triangles and points
        n_elements = (unsigned int)data.pos_indices.size();
        n_points = (unsigned int)data.positions.size();
    }

    mesh::~mesh()
    {}

    // Free all the memory in the gpu
    void mesh::destroy()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo_pos);
        glDeleteBuffers(1, &m_ebo_pos);
    }

    void mesh::bind_vao() const
    {
        glBindVertexArray(m_vao);
    }

    // Prepare the mesh to be drawn
    void mesh::draw() const
    {
        glDrawElements(GL_TRIANGLES, n_elements, GL_UNSIGNED_INT, 0);
    }

    // Draws the vertices of the mesh
    void mesh::draw_points() const
    {
        glDrawArrays(GL_POINTS, 0, n_points);
    }

    // Draws the vertices of the mesh connected by points
    void mesh::draw_segments() const
    {
        renderer::instance().wire_frame_mode(true);
        glDrawElements(GL_TRIANGLES, n_elements, GL_UNSIGNED_INT, 0);
        renderer::instance().wire_frame_mode(false);
    }
}