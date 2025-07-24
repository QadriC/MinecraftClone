#include "wireBox.hpp"

GLfloat wireVerts[] = {
        // -Z
        0.5f, -0.5f, -0.5f,
          -0.5f, -0.5f, -0.5f,
          -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        // +Z
        -0.5f, -0.5f,  0.5f,
          0.5f, -0.5f,  0.5f,
          0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f,  0.5f,
        // -X
        -0.5f, -0.5f, -0.5f,
         -0.5f, -0.5f,  0.5f,
         -0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f, -0.5f,
        // +X
         0.5f, -0.5f, 0.5f,
          0.5f, -0.5f,  -0.5f,
          0.5f,  0.5f,  -0.5f,
          0.5f,  0.5f, 0.5f, 
        // -Y
        -0.5f, -0.5f, -0.5f,
          0.5f, -0.5f, -0.5f,
          0.5f, -0.5f,  0.5f, 
         -0.5f, -0.5f,  0.5f, 
        // +Y
         -0.5f, 0.5f, 0.5f, 
          0.5f, 0.5f, 0.5f, 
          0.5f, 0.5f,  -0.5f, 
         -0.5f, 0.5f,  -0.5f
};

wireBox::wireBox() {
    vbo = new VBO(wireVerts, sizeof(wireVerts));
}

wireBox::~wireBox() {
    vao.free();
    if (vbo) {
        vbo->free();
        delete vbo;
    }
}

glm::mat4 wireBox::calc_pos(const glm::vec3 &position) {
    return glm::translate(glm::mat4(1.0f), glm::vec3(position));
}

void wireBox::render(const bool &wire) {
    vao.bind();
    vao.link_VBO(*vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    // renders a line box on top of pointed voxel
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_DEPTH_TEST);
    for (int i = 0; i < 6; ++i) {
        glDrawArrays(GL_LINE_LOOP, i * 4, 4);
    }
    glEnable(GL_DEPTH_TEST);
    // sets mode depending on state (if tab was pressed, keeps GL_LINE view)
    wire? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    vao.unbind();
    vbo->unbind();
}