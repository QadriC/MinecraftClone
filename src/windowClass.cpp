#include "windowClass.hpp"

float Window::SCREEN_WIDTH = 1920.0f;
float Window::SCREEN_HEIGHT = 1080.0f;

GLFWwindow* Window::window_init() {
    // create window
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minecraft Clone", NULL, NULL);
    
    if(!window) {
        std::cout << "Couldn't create GLFW window\n";
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    if(glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GL_TRUE);
    // set cursor disabled for FPS cam style (problematic in VM)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // setup window
    gladLoadGL();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    return window;
}

void Window::window_swap_buffers(GLFWwindow* window)  {
    // double buffering
    glfwSwapBuffers(window);
}

void Window::window_poll_events() {
    // handle events
    glfwPollEvents();
}

void Window::window_close(GLFWwindow* window) {
    // destructor
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // cursor position --> offset
    // calls functions to calculate camera angle from offset
    Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (!cam) return;
    if (cam->firstMouse) {
        cam->lastX = xpos;
        cam->lastY = ypos;
        cam->firstMouse = false;
    }

    float xoffset = xpos - cam->lastX;
    float yoffset = cam->lastY - ypos;

    cam->lastX = xpos;
    cam->lastY = ypos;

    cam->process_mouse(xoffset, yoffset);
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // interacts with voxel world by click events
    if (action != GLFW_PRESS) return;

    // get camera and world instances
    Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    extern World world;

    // camera properties at the moment
    glm::vec3 origin = cam->position;
    glm::vec3 direction = cam->front;

    // cast ray
    RaycastHit ray = world.raycast(origin, direction, 5.0f);

    if (ray.hit) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            // breack block
            world.set_block(ray.blockPos.x, ray.blockPos.y, ray.blockPos.z, BLOCK_AIR);
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            // place block
            glm::ivec3 newBlock = ray.blockPos + ray.hitNormal;
            if(world.get_block(newBlock.x, newBlock.y, newBlock.z).ID == BLOCK_AIR)
                // to make sure undesired blocks are not modified
                world.set_block(newBlock.x, newBlock.y, newBlock.z, cam->currBlock);
        }
    }
}

void Window::set_mouse_move_callback(GLFWwindow* window) {
    // set cursor callback
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, Window::mouse_callback);
}

void Window::set_mouse_click_callback(GLFWwindow* window) {
    // set click event callback
    glfwSetMouseButtonCallback(window, Window::mouse_button_callback);
}