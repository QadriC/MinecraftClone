#include "shaderClass.hpp"

// Read text file --> outputs string
std::string file_to_string(const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if(in) {
        std::string content;
        in.seekg(0, std::ios::end);
        content.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&content[0], content.size());
        in.close();
        return content;
    }

    throw std::runtime_error("Couldn't open the file " + std::string(filename));
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
    // Get file source code
    std::string vertexCode = file_to_string(vertexFile);
    std::string fragmentCode = file_to_string(fragmentFile);
    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // Create vertex shader obj
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    compile_errors(vertexShader, "VERTEX");

    // Create fragment shader obj
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    compile_errors(fragmentShader, "FRAGMENT");

    // Create shader program obj
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    compile_errors(ID, "PROGRAM");

    // Free useless obj
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::activate() {
    glUseProgram(ID);
}

void Shader::free() {
    glDeleteProgram(ID);
}

// Send mat4 --> shader
void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// Send bool --> shader
void Shader::set_bool(const std::string &name, const bool b) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), int(b));
}

// Shader error checker
void Shader::compile_errors(unsigned int shader, const char* type) {
    GLint hasCompiled;

    char infoLog[1024];
    if(type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if(hasCompiled == GL_FALSE) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR: " << type << "\n" << infoLog << "\n";
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if(hasCompiled == GL_FALSE) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_LINK_ERROR: " << type << "\n" << infoLog << "\n";
        }
    }
}