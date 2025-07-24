#ifndef WIRE_BOX_HPP
#define WIRE_BOX_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "VAO.hpp"
#include "VBO.hpp"

extern GLfloat wireVerts[];

class wireBox {
  private:
    VAO vao;
    VBO* vbo;
  
    public:
      wireBox();
      ~wireBox();
      glm::mat4 calc_pos(const glm::vec3 &position);
      void render(const bool &wire);
};

#endif