#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

class Frustum {
    private:
        glm::vec4 planes[6]; // left, right, bottom, top, near, far
        void normalizePlane(glm::vec4& plane);

    public:
        Frustum(const glm::mat4& matrix);
        bool isBoxInside(const glm::vec3& min, const glm::vec3& max) const;
};

#endif
