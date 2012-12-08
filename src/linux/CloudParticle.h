#pragma once

#include "Defines.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Utils.h"
#include <algorithm>


class CloudParticle
{
    public:
        CloudParticle(glm::vec3 pos, glm::vec3 size)
            :position(pos)
        {
            float particleSize = std::max(std::max(size.x,size.y),size.z)/1.5f;

            radius = .3f+abs(Utils::Gaussrand(particleSize+.5f, particleSize+1.f));
            position.x += Utils::Gaussrand(size.x, size.x*2);
            position.y += Utils::Gaussrand(size.y, size.y*2);
            position.z += Utils::Gaussrand(size.z, size.z*2);
        }

        ~CloudParticle(void) { };
               

        inline glm::vec3& GetPosition()
        {
            return position;
        }

        inline float GetSize()
        {
            return radius;
        }

    protected:
        float radius;
        glm::vec3 position;
};

