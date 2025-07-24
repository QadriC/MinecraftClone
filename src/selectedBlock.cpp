#include "selectedBlock.hpp"

SelectedBlock::SelectedBlock(Block b) {
    block = b;
}

SelectedBlock::~SelectedBlock() {
    vao.free();
    if (vbo) {
        vbo->free();
        delete vbo;
    }
    if (ebo) {
        ebo->free();
        delete ebo;
    }
}

void SelectedBlock::update(const Block &b) {
    if(b.ID == block.ID) return;

    block = b;

    const GLfloat faceVertices[6][20] = {
        // -Z
        { 0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
          -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
          -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
        // +Z
        { -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
          0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
          0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
         -0.5f,  0.5f,  0.5f, 0.0f, 1.0f },
        // -X
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         -0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
        // +X
        { 0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
          0.5f, -0.5f,  -0.5f, 1.0f, 0.0f,
          0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
          0.5f,  0.5f, 0.5f, 0.0f, 1.0f },
        // -Y
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
          0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
          0.5f, -0.5f,  0.5f, 1.0f, 1.0f,
         -0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
        // +Y
        { -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
          0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
          0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
         -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f }
    };

    vertices.clear();
    indices.clear();
    GLuint indexOffset = 0;
    
    int atlasW = 4, atlasH = 2;

    for (int face = 0; face < 6; ++face) {

        int tileIndex = block.get_texture_index();
        int tileX = tileIndex % atlasW;
        int tileY = tileIndex / atlasW;

        float uvW = 1.0f / float(atlasW);
        float uvH = 1.0f / float(atlasH);
        float uMin = tileX * uvW;
        float vMin = tileY * uvH;

        for (int i = 0; i < 4; ++i) {
            int base = i * 5;

            float vx = faceVertices[face][base + 0];
            float vy = faceVertices[face][base + 1];
            float vz = faceVertices[face][base + 2];

            float rawU = faceVertices[face][base + 3];
            float rawV = faceVertices[face][base + 4];

            float u = uMin + rawU * uvW;
            float v = (1.0f - (tileY + 1) * uvH) + rawV * uvH;

            vertices.push_back(vx);
            vertices.push_back(vy);
            vertices.push_back(vz);
            vertices.push_back(u);
            vertices.push_back(v);
        }

        // Add indices
        indices.push_back(indexOffset + 0);
        indices.push_back(indexOffset + 1);
        indices.push_back(indexOffset + 2);
        indices.push_back(indexOffset + 0);
        indices.push_back(indexOffset + 2);
        indices.push_back(indexOffset + 3);
        indexOffset += 4;
    }

    vao.bind();
    if (vbo) {
        vbo->free();
        delete vbo;
    }
    if (ebo) {
        ebo->free();
        delete ebo;
    }

    vbo = new VBO(vertices.data(), vertices.size() * sizeof(GLfloat));
    ebo = new EBO(indices.data(), indices.size() * sizeof(GLuint));

    vao.link_VBO(*vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);                   // Position
    vao.link_VBO(*vbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float))); // TextCoords

    vao.unbind();
}

glm::mat4 SelectedBlock::calc_pos(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &updir) {
    glm::mat4 model(1.0f);

    // calculate cam axis
    glm::vec3 forward = glm::normalize(direction);
    glm::vec3 right = glm::normalize(glm::cross(forward, updir));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    // calculate offset relative to camera position and direction
    glm::vec3 offset = right + up * -0.5f + forward * 2.0f;
    glm::vec3 finalPos = position + offset;

    // calculate model matrix
    model = glm::translate(glm::mat4(1.0f), finalPos);
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.3f));

    return model;
}

void SelectedBlock::render() {
    vao.bind();
    // renders the selected block as HUD
    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);
    
    vao.unbind();
    vbo->unbind();
    ebo->unbind();
}