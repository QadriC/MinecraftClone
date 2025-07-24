#ifndef TEXTURE_CLASS_HPP
#define TEXTURE_CLASS_HPP

#include <glad/glad.h>
#include <stb/stb_image.h>
#include "shaderClass.hpp"

class Texture {
    public:
        GLuint ID;
        GLenum type;
        Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
        void tex_unit(Shader &shader, const char* uniform, GLuint unit);
        void bind();
        void unbind();
        void free();
};

#endif