/**
* @file demo_gjk.cpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "opengl.hpp"
#include "imgui.hpp"
#include "renderer.hpp"
#include "demo_simulation.hpp"

namespace cs380 {
    /**
    * Sets everything necessary for the demo.
    * Check this function to follow the flow of the initialization of the assignment.
    */
    void demo::create()
    {
        // Set the renderer
        r = &renderer::instance();
        r->create(1920, 1080, "CS380 - Ant Simulation - Miguel Echeverria & Diego Sanz", false);

        // Set the camera
        cam = &r->get_camera();
        cam->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
        cam->set_target(glm::vec3(0.0f, 0.0f, 0.0f));

        // Set the window
        win = &r->get_window();
        
        // Set the variables of the spherical camera
        radius = 4.8f;
        alpha = 89.9f;
        beta = -90.0f;

        // Set the callback function for receiving input events
        glfwSetKeyCallback(win->handle(), key_callback);

        // Initialize Imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(win->handle(), true);
        ImGui_ImplOpenGL3_Init("#version 440");
        ImGui::StyleColorsDark();

        // Frame time variables
        current_frame_time = 0;
        prev_frame_time = 0;
        m_last_frame_time = 0;

        for (int i = 0; i < MAX_ANTS; ++i)
            m_ants[i].m_pheromoneCtr = m_pheromonesPerAnt;
    }
    
    /**
    * ImGui shutdown
    */
    void demo::destroy()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        r->destroy();
    }

    /**
    * Render ImGui
    */
    void demo::show_gui()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Options");

        // Fullscreen
        ImGui::Text("Toggle between fullscreen and windowed mode");
        if (ImGui::Button("Toggle Fullscreen"))
            win->toggle_fullscreen();
        ImGui::Separator();

        // Editing modes
        ImGui::Text("LEVEL EDIT OPTIONS:");
        ImGui::Text("- Nest Mode: Left click to place nest (only once)");
        ImGui::Text("- Food Mode: Left click to drop food");
        ImGui::Text("- Wall Mode: Left click to place walls");
        ImGui::Text("             Right click to remove walls");
        ImGui::RadioButton("Nest Mode", &m_editMode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Food Mode", &m_editMode, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Wall Mode", &m_editMode, 2);

        ImGui::End();

        ImGui::Begin("Simulation parameters");

        // Number of ants
        if (ImGui::SliderInt("Number Of Ants", &m_antNumber, 1, MAX_ANTS))
        {
            m_antNumber = glm::clamp(m_antNumber, 1, MAX_ANTS);
            m_maxPheromoneIntensity = (int)(m_pheromonesToDrop * m_antNumber * m_maxPheromoneFactor);

            reset_ants();
            m_foodGrid.Reset();
            m_pheromoneGrid.Reset();
            m_wallGrid.Reset();
            m_nestPlaced = false;
        }

        ImGui::Separator();

        //if (ImGui::SliderFloat("Simulation speed factor", &m_simulationSpeed, 0.5f, 8.0f, "%.1f"))
        //    m_simulationSpeed = glm::clamp(m_simulationSpeed, 0.5f, 8.0f);

        ImGui::Checkbox("Pause simulation", &m_isSimulationPaused);
        // Resetting the simulation
        if (ImGui::Button("Reset simulation"))
        {
            reset_ants();
            m_foodGrid.Reset();
            m_pheromoneGrid.Reset();
            m_wallGrid.Reset();
            m_nestPlaced = false;
        }

        ImGui::Separator();

        // Evaporation rate
        //ImGui::Text("Evaporation Rate");
        //if (ImGui::SliderFloat("Evaporation Rate", &m_evaporationRate, 0.0f, 0.1f, "%.4f"))
        //    m_evaporationRate = glm::clamp(m_evaporationRate, 0.0f, 1.0f);

        //if (ImGui::SliderInt("Pheromone Importance", &m_pheromoneExponent, 1, 10))
        //    m_pheromoneExponent = glm::clamp(m_pheromoneExponent, 1, 10);

       // if (ImGui::SliderInt("Distance Importance", &m_distanceExponent, 1, 10))
       //     m_distanceExponent = glm::clamp(m_distanceExponent, 1, 10);

      //  if (ImGui::SliderFloat("Ant Sample Time", &m_antSampleTime, 0.01f, 4.0f, "%.2f"))
      //      m_antSampleTime = glm::clamp(m_antSampleTime, 0.01f, 4.0f);

        if (ImGui::SliderFloat("Drop Pheromone Time", &m_dropPheromoneTime, 0.01f, 0.5f, "%.2f"))
            m_dropPheromoneTime = glm::clamp(m_dropPheromoneTime, 0.01f, 0.5f);

        if (ImGui::SliderFloat("Pheromone Decision Time", &m_pheromoneLookTime, 0.01f, 0.8f, "%.2f"))
            m_pheromoneLookTime = glm::clamp(m_pheromoneLookTime, 0.01f, 0.8f);

        //if (ImGui::SliderFloat("Max Pheromone Factor", &m_maxPheromoneFactor, 0.001f, 1.0f, "%.3f"))
        //{
        //    m_maxPheromoneFactor = glm::clamp(m_maxPheromoneFactor, 0.001f, 1.0f);
        //    m_maxPheromoneIntensity = (int)(m_pheromonesToDrop * m_antNumber * m_maxPheromoneFactor);
        //}

        if (ImGui::SliderInt("Ant FOV Radius", &m_antFovRadius, 1, 40))
            m_antFovRadius = glm::clamp(m_antFovRadius, 1, 40);

        //if (ImGui::SliderInt("Pheromones Per Ant", &m_pheromonesPerAnt, 1, 200))
        //    m_pheromonesPerAnt = glm::clamp(m_pheromonesPerAnt, 1, 200);

        if (ImGui::SliderFloat("Evaporation Time", &m_pheromoneGrid.m_pheromoneTime, 1.0f, 10.0f, "%.2f"))
            m_pheromoneGrid.m_pheromoneTime = glm::clamp(m_pheromoneGrid.m_pheromoneTime, 1.0f, 10.0f);

        ImGui::End();



        ImGui::Begin("Performance");

        // Object A properties
        ImGui::Text("FPS: %f", 1.0f / m_last_frame_time);

        ImGui::End();

        ImGui::EndFrame();
        
        // Rendering
        ImGui::Render();
    }
    
    /**
    * Renders the objects in the screen
    */
    void demo::draw()
    {
        //show_origin();

        // Draw quad
        r->bind_mesh(renderer::mesh_type::quad, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
        r->render_object(r->model_to_world(floor_pos, floor_rot, floor_scale));

        // Draw pheromones, then the nest, and then the food
        m_pheromoneGrid.DrawGrid(r);
        if (m_nestPlaced)
            draw_nest(r);
        m_foodGrid.DrawGrid(r);
        
        // Draw ants (if the nest has been placed)
        if (m_nestPlaced)
        {
            r->bind_mesh(renderer::mesh_type::pheromone, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            for (int i = 0; i < m_antNumber; ++i)
            {
                r->set_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

                // Get the rotation in the y axis
                const glm::vec3& f = m_ants[i].forward();
                float angleY = glm::degrees(atan2(f.x, f.z));
                if (angleY < 0.0f)
                    angleY += 360.0f;

                // Very necessary
                //angleY -= 90.0f;

                // Draw ant
                r->render_object(r->model_to_world(m_ants[i].pos(), glm::vec3(0.0f, angleY, 0.0f), ant_scale));

                // Draw the food it carries
                if (m_ants[i].has_food())
                {
                    r->set_color(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                    r->render_object(r->model_to_world(m_ants[i].pos() + m_ants[i].forward() * (ant_scale.z / 2.0f + FOOD_CELL_HALF_WIDTH), glm::vec3(0.0f, angleY, 0.0f), glm::vec3(FOOD_CELL_HALF_WIDTH)));
                }
            }
        }

        // Draw Walls
        m_wallGrid.DrawGrid(r);
    }

    void demo::show_origin() const
    {
        // Create the origin
        sphere origin(glm::vec3(0.0f), 0.05f);
        sphere end_point_x(glm::vec3(0.5f, 0.0f, 0.0f), 0.02f);
        sphere end_point_y(glm::vec3(0.0f, 0.5f, 0.0f), 0.02f);
        sphere end_point_z(glm::vec3(0.0f, 0.0f, 0.5f), 0.02f);

        // Create the 3 axis
        segment x_axis(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 0.0f));
        segment y_axis(glm::vec3(0.0f), glm::vec3(0.0f, 0.5f, 0.0f));
        segment z_axis(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 0.5f));

        // Colors
        glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 green(0.0f, 1.0f, 0.0f, 1.0f);
        glm::vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec4 white(1.0f, 1.0f, 1.0f, 1.0f);

        // Render
        debug_draw_segment(x_axis, red);
        debug_draw_segment(y_axis, green);
        debug_draw_segment(z_axis, blue);
        debug_draw_sphere(origin, white);
        debug_draw_sphere(end_point_x, red);
        debug_draw_sphere(end_point_y, green);
        debug_draw_sphere(end_point_z, blue);
    }

    void demo::compute_mouse_pos()
    {
        // Get the position of the mouse
        double x, y;
        glfwGetCursorPos(win->handle(), &x, &y);

        // Set the mouse pos in range [-1, 1]
        float mouse_x, mouse_y;
        mouse_x = (float)x;
        mouse_y = win->size().y - (float)y;

        // Near/far z coord
        float near_z = -1.0f;
        float far_z = 1.0f;

        // Unproject the mouse position
        const glm::mat4& model = cam->m_world_to_local;
        const glm::mat4& proj = cam->m_projection;
        glm::vec4 viewport(0.0f, 0.0f, win->size().x, win->size().y);
        glm::vec3 nearPos = glm::unProject(glm::vec3(mouse_x, mouse_y, 0.0f), model, proj, viewport);
        glm::vec3 farPos = glm::unProject(glm::vec3(mouse_x, mouse_y, 1.0f), model, proj, viewport);
        
        // Get the ray in world position
        glm::vec3 ray = glm::normalize(farPos - nearPos);

        // Raycast
        if (ray.y != 0.0f)
        {
            float t = -cam->m_pos.y / ray.y;
            mouse_pos = cam->m_pos + t * ray;
        }
    }

    // Set the position of the nest
    void demo::set_nest_pos(const glm::vec3& pos)
    {
        // Can't move it once it is placed
        if (m_nestPlaced)
            return;

        // Out of bounds
        if (pos.x >= FLOOR_SCALE_X || pos.z >= FLOOR_SCALE_Z)
            return;

        // Don't place it where walls are
        int wallRow, wallCol;
        m_wallGrid.GetRowColumn(pos, &wallRow, &wallCol);
        if (m_wallGrid.CellHasWall(wallRow, wallCol))
            return;

        m_nestPos = pos;
        m_nestPlaced = true;
        reset_ants();
    }

    void demo::simulation_logic()
    {
        if (m_editMode == 0)
            place_nest_logic();
        else if (m_editMode == 1)
            place_food_logic();
        else if (m_editMode == 2)
            place_walls_logic();

        if (!m_nestPlaced || m_isSimulationPaused)
            return;

        if (m_antSampleTimer > m_antSampleTime)
        {
            m_antSampleTimer = 0.0f;
            m_sampleAnts = true;
        }

        if (m_dropPheromoneTimer > m_dropPheromoneTime)
        {
            m_dropPheromoneTimer = 0.0f;
            m_dropPheromone = true;
        }

        if (m_pheromoneLookTimer > m_pheromoneLookTime)
        {
            m_pheromoneLookTimer = 0.0f;
            m_lookForPheromones = true;
        }

        m_pheromoneGrid.Evaporation();
        int ctr = 0;
        for (int i = 0; i < m_antNumber; ++i)
        {
            m_ants[i].update();

            if (m_dropPheromone)
            {
                if (!m_ants[i].has_food() && m_ants[i].m_pheromoneCtr)
                {
                    drop_nest_pheromone(m_ants[i].pos(), (float)m_pheromonesToDrop * m_ants[i].m_pheromoneCtr / m_pheromonesPerAnt);//(m_ants[i].m_pheromonesToDrop--));
                    --m_ants[i].m_pheromoneCtr;
                }
                else if (m_ants[i].m_pheromoneCtr)
                {
                    drop_food_pheromone(m_ants[i].pos(), (float)m_pheromonesToDrop * m_ants[i].m_pheromoneCtr / m_pheromonesPerAnt);//(m_ants[i].m_pheromonesToDrop--));
                    --m_ants[i].m_pheromoneCtr;
                }

                //if (m_ants[i].m_pheromonesToDrop < 1)
                //    m_ants[i].m_pheromonesToDrop = 1;
            }
        }

        m_dropPheromone = false;
        m_dropPheromoneTimer += dt();

        m_sampleAnts = false;
        m_antSampleTimer += dt();

        m_lookForPheromones = false;
        m_pheromoneLookTimer += dt();
    }

    void demo::drop_food_pheromone(const glm::vec3& pos, float value)
    {
        int row = 0, col = 0;
        m_pheromoneGrid.GetRowColumn(pos, &row, &col);

        if (row < 0 || col < 0 || row >= PHEROMONE_GRID_ROWS || col >= PHEROMONE_GRID_COLS)
            return;

        m_pheromoneGrid.DropFoodPheromone(row, col);//, value);
    }

    void demo::drop_nest_pheromone(const glm::vec3& pos, float value)
    {
        int row, col;
        m_pheromoneGrid.GetRowColumn(pos, &row, &col);

        if (row < 0 || col < 0 || row >= PHEROMONE_GRID_ROWS || col >= PHEROMONE_GRID_COLS)
            return;

        m_pheromoneGrid.DropNestPheromone(row, col);//, value);
    }

    void demo::place_nest_logic()
    {
        if (lclick_press)
            set_nest_pos(mouse_pos);
    }

    void demo::place_food_logic()
    {
        if (lclick_down)
        {
            m_foodGrid.DropFood(mouse_pos);
        }
    }

    void demo::place_walls_logic()
    {
        if (lclick_down)
            m_wallGrid.PlaceWall(mouse_pos);
        else if (rclick_down)
            m_wallGrid.RemoveWall(mouse_pos);
    }

    void demo::reset_ants()
    {
        for (int i = 0; i < MAX_ANTS; ++i)
            m_ants[i].reset(glm::vec3(m_nestPos.x, ANTS_Y, m_nestPos.z));
    }
    
    /**
    * Follow this function to see the flow of the assignment
    */
    bool demo::update()
    {
        // Save the time
        current_frame_time = (float)glfwGetTime();
        m_last_frame_time = current_frame_time - prev_frame_time;
        prev_frame_time = current_frame_time;

        // Check user input (camera movement and clicks)
        input();

        // Compute camera matrices
        cam->update();

        // Compute the position of the mouse in the grid
        if (lclick_press || lclick_down || rclick_press || rclick_down)
            compute_mouse_pos();

        // Ant Simulation
        simulation_logic();

        // Render the objects
        draw();

        // Render the interface of the program
        show_gui();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        return r->update_window();
    }

    /**
    * Spherical camera logic
    * WASD movement
    * E,Q closer/further
    */
    void demo::input()
    {
        // Move camera
        float cam_speed = 40.0f;

        GLFWwindow* window = win->handle();

        if (glfwGetKey(window, GLFW_KEY_W))
            alpha += 2.0f * cam_speed * dt();

        else if (glfwGetKey(window, GLFW_KEY_S))
            alpha -= 2.0f * cam_speed * dt();
        
        alpha = glm::clamp(alpha, 20.0f, 89.9f);

        if (glfwGetKey(window, GLFW_KEY_D))
            beta += 2.0f * cam_speed * dt();

        else if (glfwGetKey(window, GLFW_KEY_A))
            beta -= 2.0f * cam_speed * dt();

        glm::vec3& pos = cam->position();
        glm::vec3& view = cam->view_vec();

        pos.y = radius * sin(glm::radians(alpha));
        float new_radius = radius * cos(glm::radians(alpha));
        pos.x = new_radius * sin(glm::radians(beta));
        pos.z = new_radius * cos(glm::radians(beta));

        view = glm::normalize(-pos);

        pos = floor_pos + pos;

        // Move the camera closer/further
        if (glfwGetKey(window, GLFW_KEY_Q))
            radius += 50.0f * dt();

        else if (glfwGetKey(window, GLFW_KEY_E))
            radius -= 50.0f * dt();
        
        radius = glm::clamp(radius, 4.8f, 50.0f);

        lclick_press = false;
        lclick_down = false;
        rclick_press = false;
        rclick_down = false;

        if (ImGui::GetIO().WantCaptureMouse == false)
        {
            // Check for clicks
            curr_lclick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            curr_rclick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

            if (curr_lclick == GLFW_PRESS)
            {
                if (prev_lclick == GLFW_RELEASE)
                    lclick_press = true;
                else
                    lclick_down = true;
            }
            if (curr_rclick == GLFW_PRESS)
            {
                if (prev_rclick == GLFW_RELEASE)
                    rclick_press = true;
                else
                    rclick_down = true;
            }

            prev_lclick = curr_lclick;
            prev_rclick = curr_rclick;
        }
    }

    /**
    * Get the delta time
    */
    float demo::dt() const
    {
        return m_last_frame_time * m_simulationSpeed;
    }

    void demo::draw_nest(renderer* r)
    {
        r->bind_mesh(renderer::mesh_type::pheromone, glm::vec4(140.0f / 255.0f, 0.0f, 179.0f / 255.0f, 1.0f));

        int centerRow, centerCol;
        m_foodGrid.GetRowColumn(m_nestPos, &centerRow, &centerCol);

        if (centerRow < 0 || centerCol < 0 || centerRow >= FOOD_GRID_ROWS || centerCol >= FOOD_GRID_COLS)
            return;

        int startRow = centerRow - NEST_RADIUS;
        int startCol = centerCol - NEST_RADIUS;
        int endRow = centerRow + NEST_RADIUS;
        int endCol = centerCol + NEST_RADIUS;

        // Use food grid cells to draw the nest
        for (int row = startRow; row <= endRow; ++row)
        {
            for (int col = startCol; col <= endCol; ++col)
            {
                if (row < 0 || col < 0 || row >= FOOD_GRID_ROWS || col >= FOOD_GRID_COLS)
                    continue;

                r->render_object(r->model_to_world(m_foodGrid.GetCoords(row, col), glm::vec3(0.0f), glm::vec3(FOOD_CELL_WIDTH)));
            }
        }
    }

    /**
    * Called every time glfw detects a key press/release
    */
    void demo::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        // User wants to quit the program
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}