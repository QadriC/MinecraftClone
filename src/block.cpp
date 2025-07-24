#include "block.hpp"

BlockType blockTypes[BLOCK_COUNT] = {
    { false, -1},    // AIR
    { true, 5},      // GRASS
    { true, 4},      // DIRT
    { true, 1},      // STONE
    { true, 0},      // SAND
    { true, 2},      // PINK_LEAVES
    { true, 6},      // ORANGE_LEAVES
    { true, 3},      // LOG
    { true, 7}       // BEDROCK
};

Block::Block() {
    ID = BLOCK_AIR;
}

Block::Block(BlockID ID) {
    this->ID = ID;
}

Block::Block(int id) {
    ID = BlockID(id);
}

bool Block::is_solid() const {
    if (ID < 0 || ID >= BLOCK_COUNT) {
        // Trying to raycast out of the world
        return false;
    }
    return blockTypes[ID].solid;
}

int Block::get_texture_index() const {
    return blockTypes[ID].texture;
}