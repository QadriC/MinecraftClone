#include "cameraClass.hpp"

Camera::Camera(float width, float height) {
    // initialize every parameter
    position = glm::vec3(0.0f, 20.0f, 0.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    lateralMovementSpeed = 5.0f;
    verticalMovementSpeed = 5.0f;
    camSpeed = 60.0f;
    mouseSens = 0.1f;
    lastX = width / 2.0f;
    lastY = height / 2.0f;
    firstMouse = true;
    wireframe = false;
    tabLastFrame = false;
    currBlock = BLOCK_GRASS;
}

glm::mat4 Camera::get_view_matrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::get_projection_matrix(float aspectRatio) const {
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 500.0f);
}

void Camera::process_keyboard(GLFWwindow* window, float deltaTime) {
    float speedLM = lateralMovementSpeed * deltaTime;
    float speedVM = verticalMovementSpeed * deltaTime;
    float speedC = camSpeed * deltaTime;
    // to make easier camera movement
    glm::vec3 Front = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    glm::vec3 Right = glm::normalize(glm::cross(Front, up));

    // camera movement
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_REPEAT)
        lateralMovementSpeed = 15;
    else
        lateralMovementSpeed = 5;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += Front * speedLM;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= Front * speedLM;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= Right * speedLM;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += Right * speedLM;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += up * speedVM;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        position -= up * speedVM;
    
    // block picker
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        currBlock = BLOCK_GRASS;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        currBlock = BLOCK_DIRT;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        currBlock = BLOCK_STONE;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        currBlock = BLOCK_SAND;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        currBlock = BLOCK_LOG;
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        currBlock = BLOCK_PINK_LEAVES;
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        currBlock = BLOCK_ORANGE_LEAVES;
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        currBlock = BLOCK_BEDROCK;

    // camera angle (in case cursor doesn't work in VM users)
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        pitch += speedC;
        update();
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        pitch -= speedC;
        update();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        yaw -= speedC;
        update();
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        yaw += speedC;
        update();
    }

    // close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // toggle GL_LINE for debug
    bool tabThisFrame = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;
    if (tabThisFrame && !tabLastFrame)
        toggle_polygon();
    tabLastFrame = tabThisFrame;
}

void Camera::process_mouse(float xoffset, float yoffset) {
    // calculates new front vector (camera angle)
    // from offset input
    xoffset *= mouseSens;
    yoffset *= mouseSens;

    yaw += xoffset;
    pitch += yoffset;

    update();
}

void Camera::toggle_polygon() {
    // toggles between GL_LINE and GL_FILL for debug
    wireframe = !wireframe;
    glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);
}

void Camera::update() {
    // updates camera front
    if(pitch > 89.0f)
        pitch = 89.0f;
    else if(pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);
}