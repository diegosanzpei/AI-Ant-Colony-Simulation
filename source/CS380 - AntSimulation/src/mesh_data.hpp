/**
* @file mesh_data.hpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once

namespace cs380 {
    struct mesh_data
    {
        mesh_data();

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        std::vector<unsigned int> pos_indices;
        std::vector<unsigned int> norm_indices;
        std::vector<unsigned int> uv_indices;
    };

    mesh_data load_obj(const char* filename);
    std::vector<mesh_data> load_objs(const char* filename);
}