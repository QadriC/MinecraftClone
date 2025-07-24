#include "sea.hpp"

GLfloat seaVerts[] = {
    // Triangle 1
    -0.5f, 0.0f, -0.5f,
    -0.5f, 0.0f,  0.5f,
    0.5f, 0.0f, -0.5f,

    // Triangle 2
    -0.5f, 0.0f,  0.5f,
    0.5f, 0.0f,  0.5f,
    0.5f, 0.0f, -0.5f
};

Sea::Sea() {
    vbo = new VBO(seaVerts, sizeof(seaVerts));
}

Sea::~Sea() {
    vao.free();
    if (vbo) {
        vbo->free();
        delete vbo;
    }
}

glm::mat4 Sea::calc_pos(const glm::vec3 &position, float size) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(size, 1.0f, size));
    return model;
}

void Sea::render() {
    vao.bind();
    vao.link_VBO(*vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    // renders a plane which represents the sea
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, 12);
    
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    vao.unbind();
    vbo->unbind();
}