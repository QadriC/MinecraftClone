#ifndef SHADER_CLASS_HPP
#define SHADER_CLASS_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string file_to_string(const char* filename);

class Shader {
    public:
        GLuint ID;

        Shader(const char* vertexFile, const char* fragmentFile);
        void activate();
        void free();
        void set_mat4(const std::string &name, const glm::mat4 &mat) const;
        void set_bool(const std::string &name, const bool b) const;

    private:
        void compile_errors(unsigned int shader, const char* type);
};

#endif