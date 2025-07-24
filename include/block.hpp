#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <iostream>

enum BlockID {
    BLOCK_AIR = 0,
    BLOCK_GRASS,
    BLOCK_DIRT,
    BLOCK_STONE,
    BLOCK_SAND,
    BLOCK_PINK_LEAVES,
    BLOCK_ORANGE_LEAVES,
    BLOCK_LOG,
    BLOCK_BEDROCK,
    BLOCK_COUNT // Not a real block, sentinel
};

struct BlockType {
    bool solid;
    int texture;
};

extern BlockType blockTypes[BLOCK_COUNT];

class Block {
public:
    BlockID ID;
    Block();
    Block(BlockID ID);
    Block(int id);

    bool is_solid() const;
    int get_texture_index() const;
};

#endif