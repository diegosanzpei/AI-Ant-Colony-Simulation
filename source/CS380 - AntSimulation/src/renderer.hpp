/**
* @file renderer.hpp
* @author Diego Sanz , 540001618 , diego.sanz@digipen.edu
* @date 2020/09/20
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once

#include "math.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "geometry.hpp"
#include "mesh.hpp"

namespace cs380 {
	struct rsc
	{
		rsc();

		// Free all resources
		void destroy();

		struct mesh_rsc
		{
			mesh plane;
			mesh cube;
			mesh sphere;
		};

		struct shader_rsc
		{
			shader* phong;
		};

		mesh_rsc meshes;
		shader_rsc shaders;
	};

	class renderer {
	public:
		static renderer& instance()
		{
			static renderer inst;
			return inst;
		}

		// Core features
		void create(unsigned w, unsigned h, const char* title, bool hidden);
		void destroy();
		void load_resources();

		// Error callback for GLFW
		static void glfw_error_callback(int error, const char* description);

		// Error callback for OpenGL
		static void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
			GLsizei length, const GLchar* message, const void* userParam);
		
		// Update the window
		bool update_window();

		// Returns a reference to the program's window
		window& get_window();

		// Returns a reference of the program's camera
		camera& get_camera();

		// Returns the handle of the debug vao or vbo
		unsigned int get_debug_vbo();
		unsigned int get_debug_vao();

		// Returns a pointer to the main shader
		shader* get_main_shader();

		// Returns a reference to the resources
		rsc& resources();

		// Activates or deactivates wireframe rendering mode
		void wire_frame_mode(bool activated);

		enum class mesh_type { quad, ant, pheromone };
		void bind_mesh(const mesh_type mesh, glm::vec4 const& color);
		void set_color(glm::vec4 const& color);
		void render_object(glm::mat4 const& m2w);

		// Computes TRS
		glm::mat4 model_to_world(const glm::vec3& pos, const glm::vec3 rot, const glm::vec3& scale) const;

	private:
		renderer();
		renderer(const renderer& rhs) = delete;
		renderer& operator=(const renderer& rhs) = delete;

		// Main window of the program
		cs380::window m_window;

		// Main camera of the program
		cs380::camera m_camera;

		// Creates debug VAO and VBO
		void create_vao_vbo();

		// Frees debug VAO and VBO
		void destroy_vao_vbo();

		// Debuging VAO and VBO
		unsigned int m_debug_vao, m_debug_vbo;

		// Avoid destroying the renderer before creating it
		bool m_created;

		rsc m_resources;

		mesh* m_current_mesh;
	};
}
