/**
* @file debug.cpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "debug.hpp"
#include "geometry.hpp"
#include "opengl.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "renderer.hpp"

namespace glm {
    std::istream& operator>>(std::istream& is, vec2& v)
    {
        is >> v.x >> v.y;
        return is;
    }

    std::ostream& operator<<(std::ostream& os, vec2 const& p)
    {
        os << p.x << " " << p.y;
        return os;
    }

    std::istream& operator>>(std::istream& is, vec3& v)
    {
        is >> v.x >> v.y >> v.z;
        return is;
    }

    std::ostream& operator<<(std::ostream& os, vec3 const& p)
    {
        os << p.x << " " << p.y << " " << p.z;
        return os;
    }
}

namespace cs380 {
    // Binds the VAO/VBO and sends the mesh to the gpu
    void debug_set_data(const void* data, unsigned int size, unsigned int stride)
    {
        renderer& r = renderer::instance();

        // Bind the debug VAO
        glBindVertexArray(r.get_debug_vao());

        // Bind the debug VBO
        glBindBuffer(GL_ARRAY_BUFFER, r.get_debug_vbo());
        
        // Send the vertex to the GPU
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

        // Set the attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
        glEnableVertexAttribArray(0);
    }
    
    // Sets the color and model to projection matrix
    void debug_set_uniforms(const glm::mat4& model, const glm::vec4& color)
    {
        renderer& r = renderer::instance();

        // Get the model to projection matrix
        glm::mat4 model_to_proj = r.get_camera().get_world_to_projection() * model;

        // Set world to projection uniform
        glUniformMatrix4fv(0, 1, GL_FALSE, &model_to_proj[0][0]);

        // Set the color of the point
        glUniform4f(1, color.x, color.y, color.z, color.w);
    }

    void debug_unbind()
    {
        // Unbind the VAO and VBO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void debug_draw_point(glm::vec3 pt, glm::vec4 color)
    {
        // Send the data to the GPU and prepare everything to draw
        debug_set_data(&pt, sizeof(glm::vec3), sizeof(glm::vec3));
        debug_set_uniforms(glm::mat4(1.0f), color);

        // Draw the point
        glDrawArrays(GL_POINTS, 0, 1);

        debug_unbind();
    }

    void debug_draw_points(const mesh* mesh, glm::mat4 const& m2w, glm::vec4 color)
    {
        debug_set_uniforms(m2w, color);
        mesh->bind_vao();
        mesh->draw_points();
        debug_unbind();
    }

    void debug_draw_segment(segment const& s, glm::vec4 color)
    {
        // Send the data to the GPU and prepare everything to draw
        debug_set_data(&s, sizeof(segment), sizeof(glm::vec3));
        debug_set_uniforms(glm::mat4(1.0f), color);

        //glDisable(GL_DEPTH_TEST);
        // Draw the segment
        glDrawArrays(GL_LINES, 0, 2);
        //glEnable(GL_DEPTH_TEST);

        debug_unbind();
    }

    void debug_draw_segments(const mesh* mesh, glm::mat4 const& m2w, glm::vec4 color)
    {
        debug_set_uniforms(m2w, color);
        mesh->bind_vao();
        mesh->draw_segments();
        debug_unbind();
    }

    void debug_draw_triangle(triangle const& t, glm::vec4 color)
    {
        // Send the data to the GPU and prepare everything to draw
        debug_set_data(&t, sizeof(triangle), sizeof(glm::vec3));
        debug_set_uniforms(glm::mat4(1.0f), color);

        // Draw the traingle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        debug_unbind();
    }

    void debug_draw_plane(glm::vec3 pt, glm::vec3 n, glm::vec4 color, float scale)
    {
        glm::vec3 right = glm::cross(n, glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 m2w(1.0f);

        // Compute the model to world matrix
        if (glm::normalize(n) == glm::vec3(0.0f, 1.0f, 0.0f))
        {
            m2w = glm::translate(pt);
            m2w = glm::rotate(m2w, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            m2w = glm::scale(m2w, glm::vec3(scale));
        }

        else
            m2w = glm::inverse(glm::lookAt(pt, pt + glm::normalize(n), glm::normalize(glm::cross(right, n))));

        // Set all the necessary uniforms
        debug_set_uniforms(m2w, color);

        // Draw the mesh
        mesh& m = renderer::instance().resources().meshes.plane;
        m.bind_vao();
        m.draw();

        // Draw the back face of the plane
        glCullFace(GL_FRONT);
        debug_set_uniforms(m2w, glm::vec4(1.0f, 0.0f, 0.0f, 0.2f));
        m.draw();
        glCullFace(GL_BACK);

        // Draw the normal
        cs380::debug_draw_segment(cs380::segment(pt, pt+n), {1, 0, 0, 1});

        debug_unbind();
    }

    void debug_draw_aabb(aabb const& a, glm::vec4 color, bool wire)
    {
        // Set all the necessary uniforms
        debug_set_uniforms(a.model_to_world(), color);

        renderer::instance().resources().meshes.cube.bind_vao();

        // Draw the mesh
        if (!wire)
        {
            renderer::instance().resources().meshes.cube.draw();
        }

        else
        {
            glDisable(GL_CULL_FACE);
            renderer::instance().resources().meshes.cube.draw_segments();
            glEnable(GL_CULL_FACE);
        }

        debug_unbind();
    }

    void debug_draw_sphere(sphere const& s, glm::vec4 color)
    {
        // Set all the necessary uniforms
        debug_set_uniforms(s.model_to_world(), color);

        // Draw the mesh
        renderer::instance().resources().meshes.sphere.bind_vao();
        renderer::instance().resources().meshes.sphere.draw();
        
        debug_unbind();
    }

    void debug_draw_fancy(const mesh* mesh, glm::mat4 const& m2w, glm::vec4 color)
    {
        // Set all the necessary uniforms
        debug_set_uniforms(m2w, glm::vec4(1.0f, 1.0f, 1.0f, 0.1f));

        // Draw the mesh
        mesh->bind_vao();
        mesh->draw();

        // Draw the mesh in wire frame
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUniform4f(1, color.x, color.y, color.z, color.w);
        mesh->draw_segments();
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        debug_unbind();
    }

    void debug_draw_plain_color(const mesh* mesh, glm::mat4 const& m2w, glm::vec4 const& color)
    {
        // Set all the necessary uniforms
        debug_set_uniforms(m2w, color);

        // Draw the mesh
        mesh->bind_vao();
        mesh->draw();
        
        debug_unbind();
    }
}