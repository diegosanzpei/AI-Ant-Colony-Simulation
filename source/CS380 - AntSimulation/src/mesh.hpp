/**
* @file mesh.hpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once
#include "mesh_data.hpp"

namespace cs380 {
  class mesh
  {
  private:
  
  public:
    mesh();
  
    // Send the mesh data to the gpu
    void create(const mesh_data& data);
  
    ~mesh();
  
    // Free all the memory in the gpu
    void destroy();
  
    // Binds mesh vao
    void bind_vao() const;

    // Calls gl draw triangles
    void draw() const;
  
    // Draws the vertices of the mesh
    void draw_points() const;
  
    // Draws the vertices of the mesh connected by points
    void draw_segments() const;
  
    // OpenGL handles
    unsigned int m_vao;
    unsigned int m_vbo_pos;
    unsigned int m_ebo_pos;
    unsigned int n_elements;
    unsigned int n_points;
  };

}