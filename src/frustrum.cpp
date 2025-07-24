#include "frustrum.hpp"

Frustum::Frustum(const glm::mat4& matrix) {
    // Left
    planes[0] = glm::row(matrix, 3) + glm::row(matrix, 0);
    // Right
    planes[1] = glm::row(matrix, 3) - glm::row(matrix, 0);
    // Bottom
    planes[2] = glm::row(matrix, 3) + glm::row(matrix, 1);
    // Top
    planes[3] = glm::row(matrix, 3) - glm::row(matrix, 1);
    // Near
    planes[4] = glm::row(matrix, 3) + glm::row(matrix, 2);
    // Far
    planes[5] = glm::row(matrix, 3) - glm::row(matrix, 2);

    for (int i = 0; i < 6; ++i)
        normalizePlane(planes[i]);
}

void Frustum::normalizePlane(glm::vec4& plane) {
    float length = glm::length(glm::vec3(plane));
    if (length > 0.0f)
        plane /= length;
}

bool Frustum::isBoxInside(const glm::vec3& min, const glm::vec3& max) const {
    for (int i = 0; i < 6; ++i) {
        const glm::vec4& plane = planes[i];

        glm::vec3 positive = min;
        if (plane.x >= 0) positive.x = max.x;
        if (plane.y >= 0) positive.y = max.y;
        if (plane.z >= 0) positive.z = max.z;

        if (glm::dot(glm::vec3(plane), positive) + plane.w < 0) {
            return false; // Outside this plane
        }
    }

    return true; // Inside or intersecting all planes
}