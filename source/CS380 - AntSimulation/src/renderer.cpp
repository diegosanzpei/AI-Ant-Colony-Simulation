/**
* @file renderer.cpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "opengl.hpp"
#include "renderer.hpp"

namespace cs380{

	renderer::renderer()
		: m_debug_vao(0)
		, m_debug_vbo(0)
		, m_created(false)
		, m_current_mesh(nullptr)
	{}

	rsc::rsc()
	{
		shaders.phong = nullptr;
	}

	// Destroy all the resources
	void rsc::destroy()
	{
		// Free all meshes
		meshes.plane.destroy();
		meshes.cube.destroy();
		meshes.sphere.destroy();

		// Free the main shader
		delete shaders.phong;
	}

	/**
	 * Initializes GLFW and GLAD
	 * Creates a window with the given parameters
	 */
	void renderer::create(unsigned w, unsigned h, const char* title, bool hidden)
	{
		// Initialize GLFW
		if (!glfwInit())
		{
			// Initialization failed
			std::cout << "Failed to initialize GLFW" << std::endl;
			exit(1);
		}
		
        // Specify OpenGL version 4.4
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

		// Core profile with forward compatibility
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		// Set an error callback function for GLFW
		glfwSetErrorCallback(glfw_error_callback);

		// Create the window
		m_window.create(w, h, title, hidden);

		m_camera.set_projection(45, glm::vec2(w, h), 0.001f, 1000.0f);

        m_camera.set_position(glm::vec3(0.0f, 0.0f, 100.0f));

		// Set the current OpenGL context
		glfwMakeContextCurrent(m_window.handle());

		// Set vsync
		m_window.enable_vsync();

		// Initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
    		std::cout << "Failed to initialize GLAD" << std::endl;
			glfwTerminate();
    		exit(1);
		}

		// Set an error callback function for OpenGL
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(opengl_debug_callback, 0);

		// Set the frame buffer size
		glViewport(0, 0, w, h);

		// Set up OpenGL
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glPointSize(3.0f);

		// Clear the back buffer for the first frame
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		load_resources();
		create_vao_vbo();
		
        // Bind the shader program
        glUseProgram(m_resources.shaders.phong->GetHandle());
		m_created = true;
	}

	/**
	 * Terminates GLFW
	 */
	void renderer::destroy()
	{
		// Avoid having the renderer destroyed before creating it
		if (!m_created)
			return;
		
		destroy_vao_vbo();

		// Destroy all meshes and shaders
		m_resources.destroy();

		m_window.destroy();
		glfwTerminate();
	}

	/**
	 * Loads and creates the main shader program
	 */
	void renderer::load_resources()
	{
		// Load the main shader
		m_resources.shaders.phong = shader::CreateShaderProgram("resources/shaders/color.vert", "resources/shaders/color.frag");
		
		// Load the meshes
		m_resources.meshes.plane.create(load_obj("resources/meshes/quad.obj"));
		m_resources.meshes.cube.create(load_obj("resources/meshes/cube.obj"));
		m_resources.meshes.sphere.create(load_obj("resources/meshes/sphere.obj"));
	}

	/**
	 * Called by GLFW to notify of an error
	 */
	void renderer::glfw_error_callback(int error, const char* description)
	{
		std::cout << "GLFW Error: " << description << std::endl;
		exit(1);
	}

	void APIENTRY renderer::opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		// ignore non-significant error/warning codes
    	if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 
	
    	std::cout << "---------------" << std::endl;
    	std::cout << "Debug message (" << id << "): " <<  message << std::endl;
	
    	switch (source)
    	{
    	    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    	    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    	    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    	    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    	    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    	    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    	} std::cout << std::endl;
	
    	switch (type)
    	{
    	    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    	    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    	    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
    	    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    	    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    	    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    	    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    	    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    	    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    	} std::cout << std::endl;
	
    	switch (severity)
    	{
    	    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    	    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    	    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    	    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    	} std::cout << std::endl;
    	std::cout << std::endl;
	}

	/**
	 * Creates a VAO and VBO for debug drawing
	 */
	void renderer::create_vao_vbo()
	{
		glGenVertexArrays(1, &m_debug_vao);
		glGenBuffers(1, &m_debug_vbo);
	}

	/**
	 * Returns a pointer the main shader
	 */
	shader* renderer::get_main_shader()
	{
		return m_resources.shaders.phong;
	}

	/**
	 * Returns the handle of the debug vbo
	 */
	unsigned int renderer::get_debug_vbo()
	{
		return m_debug_vbo;
	}
	
	/**
	 * Returns the handle of the debug vao
	 */
	unsigned int renderer::get_debug_vao()
	{
		return m_debug_vao;
	}

	/**
	 * Frees the VAO and VBO
	 */
	void renderer::destroy_vao_vbo()
	{
		glDeleteBuffers(1, &m_debug_vbo);
		glDeleteVertexArrays(1, &m_debug_vao);
	}

	/**
	 * Calls the update function of the window
	 */
	bool renderer::update_window()
	{
		return m_window.update();
	}

	/**
	 * Returns a reference to the program's window
	 */
	window& renderer::get_window()
	{
		return m_window;
	}

	/**
	 * Returns a reference of the program's main camera
	 */
	camera& renderer::get_camera()
	{
		return m_camera;
	}

	/**
	 * Returns a reference to the resources
	 */
	rsc& renderer::resources()
	{
		return m_resources;
	}

	// Activates/deactivates wire frame rendering
	void renderer::wire_frame_mode(bool activated)
	{
		if (activated)
		{
        	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}

		else
		{
        	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	void renderer::bind_mesh(const mesh_type mesh, glm::vec4 const& color)
	{
		// Set the color
		glUniform4f(1, color.x, color.y, color.z, color.w);

		// Set the current mesh pointer
		switch (mesh)
		{
		case mesh_type::quad:
			m_current_mesh = &m_resources.meshes.plane;
			break;
		case mesh_type::pheromone:
			m_current_mesh = &m_resources.meshes.cube;
		}

		// Bind the vao
		m_current_mesh->bind_vao();
	}

	void renderer::set_color(glm::vec4 const& color)
	{
		// Set the color
		glUniform4f(1, color.x, color.y, color.z, color.w);
	}

	void renderer::render_object(glm::mat4 const& m2w)
	{
		// Set world to projection uniform
		glUniformMatrix4fv(0, 1, GL_FALSE, &((m_camera.get_world_to_projection() * m2w)[0][0]));

		// Draw the mesh
		m_current_mesh->draw();
	}

	glm::mat4 renderer::model_to_world(const glm::vec3& pos, const glm::vec3 rot, const glm::vec3& scale) const
	{
		glm::mat4 TRS(1.0f);

		// Translation
		TRS = glm::translate(pos);

		// Rotatation around z axis
		TRS *= glm::rotate(glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

		// Rotatation around y axis
		TRS *= glm::rotate(glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));

		// Rotatation around x axis
		TRS *= glm::rotate(glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));

		// Scale
		TRS *= glm::scale(scale);
		return TRS;
	}
}