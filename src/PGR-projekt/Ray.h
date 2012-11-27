#pragma once

#include <glm\glm.hpp>

class Ray
{
public:
    Ray(void);
    Ray(glm::vec3 origin, glm::vec3 direction);
    ~Ray(void);



protected:
    glm::vec3 origin;
    glm::vec3 direction;
};

