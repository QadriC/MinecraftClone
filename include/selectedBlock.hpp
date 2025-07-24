#ifndef SELECTED_BLOCK_HPP
#define SELECTED_BLOCK_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "block.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"

class SelectedBlock {
  private:
    VAO vao;
    VBO* vbo = nullptr;
    EBO* ebo = nullptr;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    Block block;
  
  public:
    SelectedBlock(Block block);
    ~SelectedBlock();
    glm::mat4 calc_pos(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &updir);
    void render();
    void update(const Block &b);
};

#endif