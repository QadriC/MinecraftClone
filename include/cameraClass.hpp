#ifndef CAMERA_CLASS_HPP
#define CAMERA_CLASS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include "block.hpp"

class Camera {
    public:
        // Cam attributes
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;

        // Cam angles
        float yaw;
        float pitch;

        // block held
        Block currBlock;

        // Input attributes
        float lateralMovementSpeed;
        float verticalMovementSpeed;
        float camSpeed;
        float mouseSens;
        float lastX;
        float lastY;
        bool firstMouse;
        bool wireframe;
        bool tabLastFrame;

        Camera(float width, float height);
        glm::mat4 get_view_matrix() const;
        glm::mat4 get_projection_matrix(float aspectRatio) const;

        void process_keyboard(GLFWwindow* window, float deltaTime);
        void process_mouse(float xoffset, float yoffset);
        void toggle_polygon();

    private:    
        void update();
};

#endif