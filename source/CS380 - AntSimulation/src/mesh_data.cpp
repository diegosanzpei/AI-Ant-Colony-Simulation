/**
* @file mesh_data.cpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "mesh_data.hpp"

namespace cs380 {

    mesh_data::mesh_data()
        : positions(0)
        , normals(0)
        , uvs(0)
    {}

    // Parse a given file and get all the information
    // of the mesh's vertices (pos, norm, uv)
    mesh_data load_obj(const char* filename)
    {
        // Create a mesh data
        mesh_data data;

        std::stringstream ss;
        std::ifstream in_file(filename);
        std::string line = "";
        std::string prefix = "";
        glm::vec3 temp_vec3;
        glm::vec2 temp_vec2;
        unsigned int ui1, ui2, ui3, ui4, ui5, ui6, ui7, ui8;

        // Check if the file could not be opened
        if(!in_file.is_open())
        {
            std::cout << "OBJ PARSER: File could not be opened" << std::endl;
            exit(1);
        }
        
        // Go through each line
        while(std::getline(in_file, line))
        {
            // Clear the stream
            ss.clear();

            ss.str(line);
            
            prefix.clear();

            // Get the prefix
            ss >> prefix;

            // Vertex position
            if (prefix == "v")
            {
                ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
                data.positions.push_back(temp_vec3);
            }

            // Textures coordinates
            else if (prefix == "vt")
            {
                ss >> temp_vec2.x >> temp_vec2.y;
                data.uvs.push_back(temp_vec2);
            }

            // Vertex normals
            else if (prefix == "vn")
            {
                ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
                data.normals.push_back(temp_vec3);
            }
            
            // Face
            else if (prefix == "f")
            {
                bool one_slash = false;
                bool two_slash = false;

                // Check for '/' or "//" characters
                for (unsigned int i = 0; i < line.size(); ++i)
                {
                    if (line[i] == '/')
                    {
                        one_slash = true;

                        if (line[i+1] == '/')
                            two_slash = true;

                        break;
                    }
                }

                // Set the position indices and normal indices
                if (two_slash)
                {
                    const char* ccp = line.c_str();
                    sscanf(ccp, "f%u//%u%u//%u%u//%u",&ui1,&ui4,&ui2,&ui5,&ui3,&ui6);

                    data.pos_indices.push_back (ui1 - 1);
                    data.pos_indices.push_back (ui2 - 1);
                    data.pos_indices.push_back (ui3 - 1);
                    data.norm_indices.push_back(ui4 - 1);
                    data.norm_indices.push_back(ui5 - 1);
                    data.norm_indices.push_back(ui6 - 1);
                }

                // Set the pos, norm and texture indices
                else if (one_slash)
                {
                    const char* ccp = line.c_str();
                    sscanf(ccp, 
                    "f%u/%u%u/%u%u/%u%u/%u",&ui1,&ui7,&ui2,&ui5,&ui3,&ui6,&ui4,&ui8);

                    data.pos_indices.push_back (ui1 - 1);
                    data.pos_indices.push_back (ui2 - 1);
                    data.pos_indices.push_back (ui3 - 1);
                    data.pos_indices.push_back(ui1 - 1);
                    data.pos_indices.push_back(ui3 - 1);
                    data.pos_indices.push_back(ui4 - 1);
                    data.pos_indices.push_back(ui1 - 1);
                    data.pos_indices.push_back(ui2 - 1);
                    data.pos_indices.push_back(ui3 - 1);
                    data.uv_indices.push_back  (ui4 - 1);
                    data.uv_indices.push_back  (ui5 - 1);
                    data.uv_indices.push_back  (ui6 - 1);
                    data.norm_indices.push_back(ui7 - 1);
                    data.norm_indices.push_back(ui8 - 1);
                    data.norm_indices.push_back(0);
                }

                else
                {
                    ss >> ui1 >> ui2 >> ui3;
                    data.pos_indices.push_back(ui1 - 1);
                    data.pos_indices.push_back(ui2 - 1);
                    data.pos_indices.push_back(ui3 - 1);
                }
            }
        }

        return data;
    }

    std::vector<mesh_data> load_objs(const char* filename)
    {
        return std::vector<mesh_data>();
    }
}