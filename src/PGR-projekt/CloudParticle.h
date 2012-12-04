#pragma once

#include "pgr.h"
#include <vector>
#include "Defines.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Utils.h"

#include <math.h>


typedef struct renderablePoint {
    glm::vec4 color;
    glm::vec3 position;
    glm::vec2 textCoord;
} RenderablePoint;

class CloudParticle
{
    public:
        CloudParticle(glm::vec3 pos, glm::vec3 size)
            :baseColor(.2f, .2f, .2f, .5f), position(pos)
        {
            float particleSize = std::max(std::max(size.x,size.y),size.z)/3.5;

            radius = particleSize*2+abs(Utils::Gaussrand(particleSize, particleSize));
            
            position.x += Utils::Gaussrand(size.x, size.x*3);
            position.y += Utils::Gaussrand(size.y, size.y*3);
            position.z += Utils::Gaussrand(size.z, size.z*3);                        
        }

        ~CloudParticle(void) { };

        inline const glm::vec4& GetLitColor(unsigned int index) const
        {
          if (index <= litColors.size())
            return litColors[index];
          else
            return glm::vec4(0);
        }
        
        inline glm::vec4& GetColor()
        {
            return baseColor;
        }

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
        float transparency;
        float squareSortDistance;
        glm::vec3 position;
        glm::vec4 baseColor;
        std::vector<glm::vec4> litColors;

};

