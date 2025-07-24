#include <iostream>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.hpp"
#include "textureClass.hpp"
#include "windowClass.hpp"
#include "cameraClass.hpp"
#include "world.hpp"
#include "wireBox.hpp"
#include "selectedBlock.hpp"
#include "sea.hpp"

World world;

int main() {
    GLFWwindow* window = Window::window_init();
    
    Shader defaultShader("shaders/defaultShader.vert", "shaders/defaultShader.frag");
    Shader wireShader("shaders/wireShader.vert", "shaders/wireShader.frag");
    Shader seaShader("shaders/seaShader.vert", "shaders/seaShader.frag");
    Shader selectedBlockShader("shaders/selectedBlockShader.vert", "shaders/selectedBlockShader.frag");

    Camera cam(Window::SCREEN_WIDTH, Window::SCREEN_HEIGHT);
    glfwSetWindowUserPointer(window, &cam);
    Window::set_mouse_move_callback(window);
    Window::set_mouse_click_callback(window);

    float lastFrame = 0.0f;
    float fpsTimer = 0.0f;
    int frameCount = 0;

    srand(time(nullptr) + rand());

    std::string dir = "textures/atlas.png";
    Texture atlasTex(dir.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    atlasTex.bind();

    glm::mat4 defaultProjMatrix = cam.get_projection_matrix(Window::SCREEN_WIDTH / Window::SCREEN_HEIGHT);

    defaultShader.activate();
    defaultShader.set_mat4("projection", defaultProjMatrix);
    defaultShader.set_mat4("model", glm::mat4(1.0f));

    SelectedBlock currBlock(BLOCK_AIR);
    selectedBlockShader.activate();
    selectedBlockShader.set_mat4("projection", defaultProjMatrix);

    Sea sea;
    const float seaHeight = 8.4f;
    seaShader.activate();
    seaShader.set_mat4("projection", defaultProjMatrix);

    wireBox wBox;
    wireShader.activate();
    wireShader.set_mat4("projection", defaultProjMatrix);

    // main loop
    while(!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        frameCount++;
        fpsTimer += deltaTime;

        if (fpsTimer >= 0.2f) {
            int fps = frameCount / fpsTimer;
            std::string title = "Minecraft Clone - FPS: " + std::to_string(fps);
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            fpsTimer = 0.0f;
        }

        world.update(cam.position);
        
        cam.process_keyboard(window, deltaTime);

        glClearColor(0.6, 0.8, 1.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        defaultShader.activate();
        glm::mat4 camMatrix = cam.get_view_matrix();
        defaultShader.set_mat4("view", camMatrix);
        if(cam.position.y <= seaHeight && cam.position.y >= -0.5)
            defaultShader.set_bool("underWater", true);
        else defaultShader.set_bool("underWater", false);
        world.render(camMatrix, defaultProjMatrix);

        glm::vec3 seaPos = {cam.position.x, seaHeight, cam.position.z};
        glm::mat4 seaModel = sea.calc_pos(seaPos, 150);
        seaShader.activate();
        seaShader.set_mat4("model", seaModel);
        seaShader.set_mat4("view", camMatrix);
        sea.render();
        
        glm::mat4 selectedBlockModel = currBlock.calc_pos(cam.position, cam.front, cam.up);
        selectedBlockShader.activate();
        selectedBlockShader.set_mat4("model", selectedBlockModel);
        selectedBlockShader.set_mat4("view", camMatrix);
        currBlock.update(cam.currBlock);
        currBlock.render();

        // duplicated raycast to output wirebox
        RaycastHit ray = world.raycast(cam.position, cam.front, 5.0f);
        if(ray.hit) {
            glm::mat4 wire_model = wBox.calc_pos(ray.blockPos);

            wireShader.activate();
            wireShader.set_mat4("model", wire_model);
            wireShader.set_mat4("view", camMatrix);
            wBox.render(cam.wireframe);
        }

        Window::window_swap_buffers(window);
        Window::window_poll_events();
    }
    // free memory
    world.free();
    atlasTex.free();
    defaultShader.free();
    wireShader.free();
    seaShader.free();
    selectedBlockShader.free();

    while(!world.freed) {
        // wait for the world to free every saved chunk
        // before closing gl context
    }

    Window::window_close(window);
    return 0;
}