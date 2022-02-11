/**
 * @file fwd.hpp
 * @author Eder Beldad (eder.b1@digipen.edu)
 * @date 2020-06-01
 * 
 * @copyright Copyright (c) 2020
 * @brief Forward declarations, so it they don't need to happen anywhere else
 */
#pragma once

struct GLFWwindow;

namespace cs380 {
    class mesh;
    class shader;
    class renderer;
    
    struct ray;
    struct segment;
    struct plane;
    struct triangle;
    struct aabb;
    struct sphere;
    struct frustum;
}
