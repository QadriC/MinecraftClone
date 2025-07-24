#include "world.hpp"

RaycastHit World::raycast(const glm::vec3& origin, const glm::vec3& direction, float reach) const {
    RaycastHit result;

    glm::ivec3 pos = glm::floor(origin);
    glm::vec3 deltaDist = glm::abs(glm::vec3(1.0f) / direction);

    glm::ivec3 step;
    glm::vec3 sideDist;

    for (int i = 0; i < 3; i++) {
        if (direction[i] < 0) {
            step[i] = -1;
            sideDist[i] = (origin[i] - float(pos[i])) * deltaDist[i];
        } else {
            step[i] = 1;
            sideDist[i] = (float(pos[i] + 1) - origin[i]) * deltaDist[i];
        }
    }

    float traveled = 0.0f;
    int lastAxis = -1;

    while (traveled < reach) {
        // ray out of world (vertical)
        if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
            return { false };
        
        // Check if block is solid
        if (get_block(pos.x, pos.y, pos.z).is_solid()) {
            result.hit = true;
            result.blockPos = pos;

            result.hitNormal = glm::ivec3(0);
            if (lastAxis != -1)
                result.hitNormal[lastAxis] = -step[lastAxis];

            return result;
        }

        // DDA step
        if (sideDist.x < sideDist.y) {
            if (sideDist.x < sideDist.z) {
                pos.x += step.x;
                traveled = sideDist.x;
                sideDist.x += deltaDist.x;
                lastAxis = 0;
            } else {
                pos.z += step.z;
                traveled = sideDist.z;
                sideDist.z += deltaDist.z;
                lastAxis = 2;
            }
        } else {
            if (sideDist.y < sideDist.z) {
                pos.y += step.y;
                traveled = sideDist.y;
                sideDist.y += deltaDist.y;
                lastAxis = 1;
            } else {
                pos.z += step.z;
                traveled = sideDist.z;
                sideDist.z += deltaDist.z;
                lastAxis = 2;
            }
        }
    }

    return result;
}


World::World() {
    freed = false;
}

void World::update(const glm::vec3& playerPos) {
    int playerChunkX = (int)std::floor(playerPos.x / CHUNK_SIZE);
    int playerChunkZ = (int)std::floor(playerPos.z / CHUNK_SIZE);

    std::unordered_map<std::pair<int, int>, Chunk*, pair_hash> newActiveChunks;
    // get chunks around player in render distance radius
    // and store them in new unordered map
    for (int dx = -renderDistance; dx <= renderDistance; ++dx) {
        for (int dz = -renderDistance; dz <= renderDistance; ++dz) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;

            if (!get_chunk(chunkX, chunkZ)) {
                load_chunk(chunkX, chunkZ);
            }

            newActiveChunks[{chunkX, chunkZ}] = get_chunk(chunkX, chunkZ);
        }
    }
    // save the new unordered map as activeChunks
    activeChunks = std::move(newActiveChunks);
}

void World::render(const glm::mat4 &view, const glm::mat4 &projection) {
    // draw active chunks which are inside the camera frustrum
    Frustum frustum(projection * view);

    for (const auto& pair : activeChunks) {
        Chunk* chunk = pair.second;

        glm::vec3 chunkMin(pair.first.first * CHUNK_SIZE, 0.0f, pair.first.second * CHUNK_SIZE);
        glm::vec3 chunkMax = chunkMin + glm::vec3(CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE);

        if (frustum.isBoxInside(chunkMin, chunkMax)) {
            chunk->draw();
        }
    }
}

void World::load_chunk(int chunkX, int chunkZ) {
    auto chunk = std::make_unique<Chunk>(chunkX, chunkZ);
    Chunk* rawChunk = chunk.get();
    worldChunks[{chunkX, chunkZ}] = std::move(chunk);

    // get neighbor chunks
    Chunk* left  = get_chunk(chunkX - 1, chunkZ);
    Chunk* right = get_chunk(chunkX + 1, chunkZ);
    Chunk* front = get_chunk(chunkX, chunkZ + 1);
    Chunk* back  = get_chunk(chunkX, chunkZ - 1);

    generate_all_trees();

    // build actual chunk's mesh
    rawChunk->build_mesh(left, right, front, back);

    // rebuild neighbor chunks' mesh to remove border faces
    if (left)  left->build_mesh(get_chunk(chunkX - 2, chunkZ), rawChunk, get_chunk(chunkX - 1, chunkZ + 1), get_chunk(chunkX - 1, chunkZ - 1));
    if (right) right->build_mesh(rawChunk, get_chunk(chunkX + 2, chunkZ), get_chunk(chunkX + 1, chunkZ + 1), get_chunk(chunkX + 1, chunkZ - 1));
    if (front) front->build_mesh(get_chunk(chunkX - 1, chunkZ + 1), get_chunk(chunkX + 1, chunkZ + 1), get_chunk(chunkX, chunkZ + 2), rawChunk);
    if (back)  back->build_mesh(get_chunk(chunkX - 1, chunkZ - 1), get_chunk(chunkX + 1, chunkZ - 1), rawChunk, get_chunk(chunkX, chunkZ - 2));
}


Chunk* World::get_chunk(int chunkX, int chunkZ) {
    auto it = worldChunks.find({chunkX, chunkZ});
    if (it != worldChunks.end()) {
        // found, return chunk
        return it->second.get();
    }
    
    return nullptr;
}

Block World::get_block(int worldX, int worldY, int worldZ) const {
    // world coords --> chunk number & chunk coords
    int chunkX = (worldX >= 0) ? worldX / CHUNK_SIZE : ((worldX + 1) / CHUNK_SIZE) - 1;
    int chunkZ = (worldZ >= 0) ? worldZ / CHUNK_SIZE : ((worldZ + 1) / CHUNK_SIZE) - 1;

    int localX = (worldX % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;
    int localZ = (worldZ % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;

    auto it = activeChunks.find({chunkX, chunkZ});
    if (it != activeChunks.end()) {
        // chunk active, return block in specified chunk coords
        return it->second->get_block(localX, worldY, localZ);
    }
    // chunk not active (shouldn't happen with current implementation)
    return Block(BLOCK_AIR);
}

void World::set_block(int worldX, int worldY, int worldZ, Block block) {
    // world coords --> chunk number & chunk coords
    int chunkX = (worldX >= 0) ? worldX / CHUNK_SIZE : ((worldX + 1) / CHUNK_SIZE) - 1;
    int chunkZ = (worldZ >= 0) ? worldZ / CHUNK_SIZE : ((worldZ + 1) / CHUNK_SIZE) - 1;

    int localX = (worldX % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;
    int localZ = (worldZ % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;

    Chunk* chunk = get_chunk(chunkX, chunkZ);
    if (chunk) {
        // chunk active, set block in specific chunk coords
        chunk->set_block(localX, worldY, localZ, block);

        Chunk* left  = get_chunk(chunkX - 1, chunkZ);
        Chunk* right = get_chunk(chunkX + 1, chunkZ);
        Chunk* front = get_chunk(chunkX, chunkZ + 1);
        Chunk* back  = get_chunk(chunkX, chunkZ - 1);

        chunk->build_mesh(left, right, front, back);

        // block placed in a chunk border, rebuild neighbor
        if (localX == 0 && left)   left->build_mesh(get_chunk(chunkX - 2, chunkZ), chunk, get_chunk(chunkX - 1, chunkZ + 1), get_chunk(chunkX - 1, chunkZ - 1));
        if (localX == CHUNK_SIZE - 1 && right) right->build_mesh(chunk, get_chunk(chunkX + 2, chunkZ), get_chunk(chunkX + 1, chunkZ + 1), get_chunk(chunkX + 1, chunkZ - 1));
        if (localZ == 0 && back)   back->build_mesh(get_chunk(chunkX - 1, chunkZ - 1), get_chunk(chunkX + 1, chunkZ - 1), chunk, get_chunk(chunkX, chunkZ - 2));
        if (localZ == CHUNK_SIZE - 1 && front) front->build_mesh(get_chunk(chunkX - 1, chunkZ + 1), get_chunk(chunkX + 1, chunkZ + 1), get_chunk(chunkX, chunkZ + 2), chunk);
    }
}

void World::generate_all_trees() {
    for (auto& [pos, chunk] : worldChunks) {
        int x = pos.first;
        int z = pos.second;

        Chunk* left  = get_chunk(x - 1, z);
        Chunk* right = get_chunk(x + 1, z);
        Chunk* back  = get_chunk(x, z - 1);
        Chunk* front = get_chunk(x, z + 1);

        chunk->generate_trees(left, right, front, back);
    }
}

void World::free() {
    activeChunks.clear();
    worldChunks.clear();
    freed = true;
}