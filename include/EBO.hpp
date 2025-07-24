#ifndef EBO_HPP
#define EBO_HPP

#include <glad/glad.h>

class EBO {
    public:
        GLuint ID;

        EBO(GLuint* indices, GLsizeiptr size);

        void bind();
        void unbind();
        void free();
};

#endif