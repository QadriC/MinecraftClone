#ifndef VBO_HPP
#define VBO_HPP

#include <glad/glad.h>

class VBO {
    public:
        GLuint ID;

        VBO(GLfloat* vertices, GLsizeiptr size);

        void bind();
        void unbind();
        void free();
};

#endif