#ifndef VAO_HPP
#define VAO_HPP

#include <glad/glad.h>
#include "VBO.hpp"

class VAO {
    public:
        GLuint ID;

        VAO();

        void link_VBO(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
        void bind();
        void unbind();
        void free();
};

#endif