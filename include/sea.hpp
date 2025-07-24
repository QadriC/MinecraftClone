#ifndef SEA_HPP
#define SEA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "VAO.hpp"
#include "VBO.hpp"

extern GLfloat seaVerts[];

class Sea {
  private:
    VAO vao;
    VBO* vbo;
  
    public:
      Sea();
      ~Sea();
      glm::mat4 calc_pos(const glm::vec3 &position, float size);
      void render();
};

#endif