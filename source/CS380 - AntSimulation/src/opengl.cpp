#include "pch.hpp"
#include "opengl.hpp"
#include "lodepng.h"

namespace{
	/**
	 * OpenGL callback for debugging
	 * @param source
	 * @param type
	 * @param id
	 * @param severity
	 * @param length
	 * @param message
	 * @param userParam
	 */
	void APIENTRY openglCallbackFunction(GLenum source,
										 GLenum type,
										 GLuint id,
										 GLenum severity,
										 GLsizei /* length */,
										 const GLchar* message,
										 const void* /* userParam */)
	{
		
	}
}

namespace cs350{
	/**
	 * Enables OpenGL callbacks. This callbacks will be useful to intercept errors.
	 */
	void enable_gl_callbacks()
	{
		
	}

	/**
	 * Takes an screenshot
	 * @param width
	 * @param height
	 * @return
	 */
	std::vector<glm::vec<4, unsigned char>> take_screenshoot(unsigned width, unsigned height)
	{
		std::vector<glm::vec<4, unsigned char>> result;
		result.resize(width * height, {});
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadBuffer(GL_FRONT);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, result.data());
		// Flip vertically (inefficiently)
		for (unsigned x = 0; x < width; ++x){
			for (unsigned y = 0; y < height / 2; ++y){
				std::swap(result.at(y * width + x), result.at((height - 1 - y) * width + x));
			}
		}
		return result;
	}

	/**
	 * Saves an screenshoot to a BMP
	 * @param width
	 * @param height
	 * @param filename
	 * @return
	 */
	std::vector<glm::vec<4, unsigned char>> save_screenshoot(unsigned width, unsigned height, std::string const& filename)
	{
		auto pixels = take_screenshoot(width, height);

		auto data = &(pixels.front()[0]);
		auto err  = lodepng::encode(filename, data, width, height);
		if (err){
			std::cerr << "Lodepng failed to save file " << filename;
			exit(-1);
		}		
		return pixels;
	}
}