#ifndef WINDOW_CLASS_HPP
#define WINDOW_CLASS_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "cameraClass.hpp"
#include "world.hpp"

class Window {
    public:
        static float SCREEN_WIDTH;
        static float SCREEN_HEIGHT;
    
        static GLFWwindow* window_init();
        static void window_swap_buffers(GLFWwindow* window);
        static void window_poll_events();
        static void window_close(GLFWwindow* window);
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void set_mouse_move_callback(GLFWwindow* window);
        static void set_mouse_click_callback(GLFWwindow* window);
};

#endif