#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>
#include "FastNoiseLite.hpp"
#include "block.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"

const int CHUNK_SIZE = 8;
const int CHUNK_HEIGHT = 30;

class Chunk {
public:
    Chunk(int x, int z);
    ~Chunk();

    float octave_noise(float x, float z, FastNoiseLite& noise);
    void generate_blocks();
    void generate_trees(Chunk* left, Chunk* right, Chunk* front, Chunk* back);
    int get_ao(bool side1, bool side2, bool corner);
    bool is_block_solid_at(int x, int y, int z, Chunk* left, Chunk* right, Chunk* front, Chunk* back) const;
    void build_mesh(Chunk* left, Chunk* right, Chunk* front, Chunk* back);
    void draw();

    Block get_block(int x, int y, int z) const;
    void set_block(int x, int y, int z, Block block);
    void set_block_safe(int x, int y, int z, Block block, Chunk* left, Chunk* right, Chunk* front, Chunk* back);

private:
    int chunkX, chunkZ;
    std::vector<Block> blocks;
    bool treesGenerated;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    VAO vao;
    VBO* vbo = nullptr;
    EBO* ebo = nullptr;

    int get_index(int x, int y, int z) const;
};

#endif
