#pragma once

#include <glm\glm.hpp>

class Ray
{
public:
    Ray(void);
    Ray(glm::vec3 origin, glm::vec3 direction);
    ~Ray(void);

    glm::vec3 GetDirection() { return direction; }
    glm::vec3 GetOrigin() { return origin; }


protected:
    glm::vec3 origin;
    glm::vec3 direction;
};

