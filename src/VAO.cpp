#include "VAO.hpp"

VAO::VAO() {
    glGenVertexArrays(1, &ID);
}

void VAO::link_VBO(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
    VBO.bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    VBO.unbind();
}

void VAO::bind() {
    glBindVertexArray(ID);
}

void VAO::unbind() {
    glBindVertexArray(0);
}

void VAO::free() {
    if(glIsVertexArray(ID) && ID != 0) {
        glDeleteVertexArrays(1, &ID);
        ID = 0;
    }
}