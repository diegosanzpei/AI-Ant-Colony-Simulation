/**
* @file window.cpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "opengl.hpp"
#include "window.hpp"

namespace cs380{
	bool window::create(int w, int h, const char* window_name, bool hidden)
    {
        //Specify if the window should be visible
        glfwWindowHint(GLFW_VISIBLE, !hidden);

        // Window can't be resizable
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // Create the window and set the window pointer
        m_window = glfwCreateWindow(w, h, window_name, NULL, NULL);

        if (!m_window)
        {
            std::cout << "Window or OpenGL context creation failed" << std::endl;
            glfwTerminate();
            exit(1);
        }

        m_size.x = w;
        m_size.y = h;

        return true;
    }

    bool window::update()
    {
        // Process events
        glfwPollEvents();

        // Keep Running
        if(!glfwWindowShouldClose(m_window))
        {
            // Show the rendered frame
            glfwSwapBuffers(m_window);

            // Clear the back buffer for the next frame
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            return true;
        }

        // Close the window
        return false;
    }

    void window::destroy()
    {
        glfwDestroyWindow(m_window);
    }

    void window::enable_vsync()
    {
        // Enable vsync
        glfwSwapInterval(1);
    }

    void window::toggle_fullscreen()
    {
        fullscreen = !fullscreen;
        glfwSetWindowMonitor(m_window, fullscreen ? glfwGetPrimaryMonitor() : NULL, 50, 50, m_size.x, m_size.y, GLFW_DONT_CARE);
        enable_vsync();
    }
}