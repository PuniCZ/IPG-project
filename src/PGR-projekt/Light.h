#pragma once

#include "Utils.h"

class Light
{
    public:
        Light(glm::vec3 pos, glm::vec3 dir, glm::vec4 cDiffuse, glm::vec4 cAmbient, glm::vec4 cSpecular);
        ~Light(void);

        glm::vec3 GetDirection() { return direction; }
        glm::vec4 GetDiffuse() { return colorDiffuse; }
        glm::vec4 GetAmbient() { return colorAmbient; }
        glm::vec4 GetSpecular() { return colorSpecular; }

    protected:
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec4 colorDiffuse;
        glm::vec4 colorAmbient;
        glm::vec4 colorSpecular;

};

