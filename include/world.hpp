#ifndef WORLD_HPP
#define WORLD_HPP

#include <iostream>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

#include "chunk.hpp"
#include "frustrum.hpp"

struct RaycastHit {
    bool hit = false;
    glm::ivec3 blockPos;
    glm::ivec3 hitNormal; // block's face hit
};

class World {
private:
    struct pair_hash {
        std::size_t operator()(const std::pair<int, int>& p) const {
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };

    int renderDistance = 8;
    std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, pair_hash> worldChunks;
    std::unordered_map<std::pair<int, int>, Chunk*, pair_hash> activeChunks;

    
    Chunk* get_chunk(int chunkX, int chunkZ);
    void load_chunk(int chunkX, int chunkZ);

public:
    World();
    void free();

    void update(const glm::vec3& playerPos);
    void render(const glm::mat4 &view, const glm::mat4 &projection);

    Block get_block(int worldX, int worldY, int worldZ) const;
    void set_block(int worldX, int worldY, int worldZ, Block block);
    void generate_all_trees();
    RaycastHit raycast(const glm::vec3& origin, const glm::vec3& direction, float reach) const;

    bool freed;
};


#endif