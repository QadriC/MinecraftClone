#include "chunk.hpp"

Chunk::Chunk(int x, int z) : chunkX(x), chunkZ(z) {
    blocks.resize(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE);
    treesGenerated = false;
    generate_blocks();
}

Chunk::~Chunk() {
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

int Chunk::get_index(int x, int y, int z) const {
    // returns block index in blocks array from chunk coords
    if(x < 0 || x >= CHUNK_SIZE) {
        return -1;
    }

    if(y < 0 || y >= CHUNK_HEIGHT) {
        return -1;
    }

    if(z < 0 || z >= CHUNK_SIZE) {
        return -1;
    }

    return x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT;
}

Block Chunk::get_block(int x, int y, int z) const {
    // returns block id from chunk coords
    int index = get_index(x, y, z);
    if(index == -1)
        return BLOCK_AIR;
    return blocks[index];
}

void Chunk::set_block(int x, int y, int z, Block block) {
    int index = get_index(x, y, z);
    if(index == -1) return;
    blocks[index] = block;
}

void Chunk::set_block_safe(int x, int y, int z, Block block, Chunk* left, Chunk* right, Chunk* front, Chunk* back) {
    if (y < 0 || y >= CHUNK_HEIGHT) return;
    if (x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
        this->set_block(x, y, z, block);
    } else if (x < 0 && left) {
        left->set_block(x + CHUNK_SIZE, y, z, block);
    } else if (x >= CHUNK_SIZE && right) {
        right->set_block(x - CHUNK_SIZE, y, z, block);
    } else if (z < 0 && back) {
        back->set_block(x, y, z + CHUNK_SIZE, block);
    } else if (z >= CHUNK_SIZE && front) {
        front->set_block(x, y, z - CHUNK_SIZE, block);
    }
}

float Chunk::octave_noise(float x, float z, FastNoiseLite& noise) {
    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < 4; ++i) {
        total += noise.GetNoise(x * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    return total / maxValue;
}

void Chunk::generate_blocks() {
    // chunk generation
    static int seed = std::rand();
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.01f);
    noise.SetSeed(seed);

    const float baseFrequency = 0.5f;
    const int SEA_LEVEL = 8;

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            int worldX = chunkX * CHUNK_SIZE + x;
            int worldZ = chunkZ * CHUNK_SIZE + z;

            float heightNoise = octave_noise(worldX * baseFrequency, worldZ * baseFrequency, noise);
            int height = (int)((heightNoise + 1.0f) * 0.5f * (CHUNK_HEIGHT - 1));
            
            for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                if (y > height)
                    set_block(x, y, z, BLOCK_AIR);
                else if (y == height){
                    if (y <= SEA_LEVEL)
                        set_block(x, y, z, BLOCK_SAND);
                    else
                        set_block(x, y, z, BLOCK_GRASS);
                }
                else if (y == 0) {
                    set_block(x, y, z, BLOCK_BEDROCK);
                }
                else if (y > height - 3)
                    set_block(x, y, z, BLOCK_DIRT);
                else
                    set_block(x, y, z, BLOCK_STONE); 
            }
        }
    }
}

void Chunk::generate_trees(Chunk* left, Chunk* right, Chunk* front, Chunk* back) {
    if (treesGenerated) return;

    treesGenerated = true;
    static int seed = std::rand();
    FastNoiseLite treeNoise;
    treeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    treeNoise.SetFrequency(4.0f);
    treeNoise.SetSeed(seed + 5000);

    for (int x = 1; x < CHUNK_SIZE - 1; ++x) {
        for (int z = 1; z < CHUNK_SIZE - 1; ++z) {
            int worldX = chunkX * CHUNK_SIZE + x;
            int worldZ = chunkZ * CHUNK_SIZE + z;

            float treeChance = treeNoise.GetNoise((float)worldX, (float)worldZ);
            if (treeChance > 0.92f) {
                // find surface block below limit -10 (max tree size)
                for (int y = CHUNK_HEIGHT - 10; y >= 1; --y) {
                    Block current = get_block(x, y, z);
                    // found surface block
                    if (current.ID == BLOCK_GRASS) {
                        // change it to dirt
                        set_block(x, y, z, BLOCK_DIRT);
                        
                        // trunk height between 4 - 7
                        int trunkHeight = 4 + std::rand() % 4;

                        // place trunk
                        for (int h = 1; h <= trunkHeight; ++h) {
                            int blockY = y + h;
                            if (blockY >= 0 && blockY < CHUNK_HEIGHT) {
                                set_block_safe(x, blockY, z, BLOCK_LOG, left, right, front, back);
                            }
                        }
                        // leafType (1 or 2)
                        Block leafType;
                        rand() % 2 == 0? leafType = BLOCK_PINK_LEAVES : leafType = BLOCK_ORANGE_LEAVES;
                        // leaves center
                        int leafCenterY = y + trunkHeight;

                        // leaves sphere radius
                        int radius = 3;

                        for (int dy = -radius; dy <= radius; ++dy) {
                            int blockY = leafCenterY + dy;
                            if (blockY < 0 || blockY >= CHUNK_HEIGHT) continue;
                            for (int dx = -radius; dx <= radius; ++dx) {
                                for (int dz = -radius; dz <= radius; ++dz) {
                                    float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

                                    if (dist <= radius) {
                                        if (dx == 0 && dz == 0 && blockY >= y && blockY <= y + trunkHeight)
                                            continue;
                                        if(blockY == leafCenterY) continue;
                                        // skip random leaves (avoid perfect sphere)
                                        if (std::rand() % 100 < 75) { // 25% to skip leaf
                                            
                                            set_block_safe(x + dx, leafCenterY + dy, z + dz, leafType, left, right, front, back);
                                        }
                                    }
                                }
                            }
                        }
                        // tree placed, stop looking for surface block
                        break;
                    }
                }
            }
        }
    }
}

int Chunk::get_ao(bool side1, bool side2, bool corner) {
    if (side1 && side2) return 0;
    return 3 - (int(side1) + int(side2) + int(corner));
}

bool Chunk::is_block_solid_at(int x, int y, int z, Chunk* left, Chunk* right, Chunk* front, Chunk* back) const {
    if (x < 0) {
        if (!left) return false;
        return left->get_block(CHUNK_SIZE - 1, y, z).is_solid();
    }
    if (x >= CHUNK_SIZE) {
        if (!right) return false;
        return right->get_block(0, y, z).is_solid();
    }
    if (z < 0) {
        if (!back) return false;
        return back->get_block(x, y, CHUNK_SIZE - 1).is_solid();
    }
    if (z >= CHUNK_SIZE) {
        if (!front) return false;
        return front->get_block(x, y, 0).is_solid();
    }
    if (y < 0 || y >= CHUNK_HEIGHT) return false;

    return get_block(x, y, z).is_solid();
}


void Chunk::build_mesh(Chunk* left, Chunk* right, Chunk* front, Chunk* back) {
    // constructs chunk's vao, vbo and ebo
    vertices.clear();
    indices.clear();
    GLuint indexOffset = 0;

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

    const int neighborOffsets[6][3] = {
        {  0,  0, -1 }, // -Z
        {  0,  0,  1 }, // +Z
        { -1,  0,  0 }, // -X
        {  1,  0,  0 }, // +X
        {  0, -1,  0 }, // -Y
        {  0,  1,  0 }  // +Y
    };

    // hardcoded blocks that each vertex has to check to calculate AO
    // format: 6 Faces --> 4 Vertex --> 3 Blocks to calculate --> X, Y, Z
    //                                   - side 1
    //                                   - side 2
    //                                   - corner
    const int aoOffsets[6][4][3][3] = {
        // -Z
        {{{ 0,-1,-1}, { 1, 0,-1}, { 1,-1,-1}},  // v0
        {{ 0,-1,-1}, {-1, 0,-1}, {-1,-1,-1}},  // v1 
        {{ 0, 1,-1}, {-1, 0,-1}, {-1, 1,-1}},  // v2 
        {{ 0, 1,-1}, { 1, 0,-1}, { 1, 1,-1}}}, // v3

        // +Z
        {{{ 0,-1, 1}, {-1, 0, 1}, {-1,-1, 1}},  // v0 
        {{ 0,-1, 1}, { 1, 0, 1}, { 1,-1, 1}},  // v1
        {{ 0, 1, 1}, { 1, 0, 1}, { 1, 1, 1}},  // v2
        {{ 0, 1, 1}, {-1, 0, 1}, {-1, 1, 1}}}, // v3 

        // -X
        {{{ -1,0, -1}, { -1, -1, 0}, { -1, -1, -1}},  // v0 
        {{ -1,0, 1}, { -1, -1, 0}, { -1, -1,1}},  // v1 
        {{ -1, 0, 1}, { -1, 1, 0}, { -1, 1, 1}},  // v2 
        {{ -1, 0,-1}, { -1, 1, 0}, { -1, 1, -1}}}, // v3 

        // +X
        {{{ 1, 0, 1}, { 1, -1, 0}, { 1, -1, 1}},  // v0
        {{ 1, 0, -1}, { 1, -1, 0}, { 1, -1, -1}},  // v1
        {{ 1, 0, -1}, { 1, 1, 0}, { 1, 1, -1}},  // v2
        {{ 1, 0, 1}, { 1, 1, 0}, { 1, 1, 1}}}, // v3

        // -Y
        {{{-1,-1, 0}, { 0,-1,-1}, {-1,-1,-1}},  // v0
        {{ 1,-1, 0}, { 0,-1,-1}, { 1,-1,-1}},  // v1 
        {{ 1,-1, 0}, { 0,-1, 1}, { 1,-1, 1}},  // v2
        {{-1,-1, 0}, { 0,-1, 1}, {-1,-1, 1}}}, // v3 

        // +Y
        {{{-1, 1, 0}, { 0, 1, 1}, {-1, 1, 1}},  // v0
        {{ 1, 1, 0}, { 0, 1, 1}, { 1, 1, 1}},  // v1
        {{ 1, 1, 0}, { 0, 1,-1}, { 1, 1,-1}},  // v2
        {{-1, 1, 0}, { 0, 1,-1}, {-1, 1,-1}}}  // v3 
    };

    const int atlasW = 4;
    const int atlasH = 2;

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_HEIGHT; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                const Block& block = get_block(x, y, z);
                if (!block.is_solid()) continue;

                float fx = float(x + chunkX * CHUNK_SIZE);
                float fy = float(y);
                float fz = float(z + chunkZ * CHUNK_SIZE);

                // face skipping depending on neighbor blocks
                for (int face = 0; face < 6; ++face) {
                    int nx = x + neighborOffsets[face][0];
                    int ny = y + neighborOffsets[face][1];
                    int nz = z + neighborOffsets[face][2];

                    bool drawFace = false;

                    switch (face) {
                        case 0: drawFace = (z == 0) ? !back || !back->get_block(x, y, CHUNK_SIZE - 1).is_solid() : !get_block(x, y, z - 1).is_solid(); break;
                        case 1: drawFace = (z == CHUNK_SIZE - 1) ? !front || !front->get_block(x, y, 0).is_solid() : !get_block(x, y, z + 1).is_solid(); break;
                        case 2: drawFace = (x == 0) ? !left || !left->get_block(CHUNK_SIZE - 1, y, z).is_solid() : !get_block(x - 1, y, z).is_solid(); break;
                        case 3: drawFace = (x == CHUNK_SIZE - 1) ? !right || !right->get_block(0, y, z).is_solid() : !get_block(x + 1, y, z).is_solid(); break;
                        case 4: drawFace = (y == 0) ? true : !get_block(x, y - 1, z).is_solid(); break;
                        case 5: drawFace = (y == CHUNK_HEIGHT - 1) ? true : !get_block(x, y + 1, z).is_solid(); break;
                    }

                    if (!drawFace) continue;

                    int tileIndex = block.get_texture_index();
                    int tileX = tileIndex % atlasW;
                    int tileY = tileIndex / atlasW;

                    float uvW = 1.0f / float(atlasW);
                    float uvH = 1.0f / float(atlasH);
                    float uMin = tileX * uvW;
                    float vMin = tileY * uvH;

                    // Add face vertices (with offset) adjusting atlas UV + AO calculation by offsets
                    for (int i = 0; i < 4; ++i) {
                        int base = i * 5;

                        float vx = faceVertices[face][base + 0] + fx;
                        float vy = faceVertices[face][base + 1] + fy;
                        float vz = faceVertices[face][base + 2] + fz;

                        float rawU = faceVertices[face][base + 3];
                        float rawV = faceVertices[face][base + 4];

                        float u = uMin + rawU * uvW;
                        float v = (1.0f - (tileY + 1) * uvH) + rawV * uvH;

                        // neighbor AOs
                        int ao1x = x + aoOffsets[face][i][0][0];
                        int ao1y = y + aoOffsets[face][i][0][1];
                        int ao1z = z + aoOffsets[face][i][0][2];

                        int ao2x = x + aoOffsets[face][i][1][0];
                        int ao2y = y + aoOffsets[face][i][1][1];
                        int ao2z = z + aoOffsets[face][i][1][2];

                        int ao3x = x + aoOffsets[face][i][2][0];
                        int ao3y = y + aoOffsets[face][i][2][1];
                        int ao3z = z + aoOffsets[face][i][2][2];

                        bool s1 = is_block_solid_at(ao1x, ao1y, ao1z, left, right, front, back);
                        bool s2 = is_block_solid_at(ao2x, ao2y, ao2z, left, right, front, back);
                        bool c  = is_block_solid_at(ao3x, ao3y, ao3z, left, right, front, back);

                        float ao = get_ao(s1, s2, c) / 3.0f;

                        vertices.push_back(vx);
                        vertices.push_back(vy);
                        vertices.push_back(vz);
                        vertices.push_back(u);
                        vertices.push_back(v);
                        vertices.push_back(ao);
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
            }
        }
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

    vao.link_VBO(*vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);                   // Position
    vao.link_VBO(*vbo, 1, 2, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float))); // TextCoords
    vao.link_VBO(*vbo, 2, 1, GL_FLOAT, 6 * sizeof(float), (void*)(5 * sizeof(float))); // AO 
    vao.unbind();
}

void Chunk::draw() {
    vao.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
