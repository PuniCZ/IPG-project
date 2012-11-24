#pragma once

#include "pgr.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define RAND(max) (float(rand())/RAND_MAX)*max

typedef struct renderablePoint {
    glm::vec4 color;
    glm::vec3 position;
} RenderablePoint;

class CloudParticle
{
    public:
        CloudParticle(float color)
            :baseColor(RAND(1), RAND(1), RAND(1), 1.f)
        {
            //TEMP
            radius = RAND(0.5)+.02f;
            position.x += 10-RAND(20);
            position.y += 10-RAND(20);
            position.z += 10-RAND(20);


            Update();
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

        inline glm::vec3& GetPointPosition()
        {
            return position;
        }

        void Update()
        {            
            //front
            RenderablePoints[0].position.x = position.x - radius/2.f;
            RenderablePoints[0].position.y = position.y - radius/2.f;
            RenderablePoints[0].position.z = position.z;

            RenderablePoints[1].position.x = position.x + radius/2.f;
            RenderablePoints[1].position.y = position.y - radius/2.f;
            RenderablePoints[1].position.z = position.z;

            RenderablePoints[2].position.x = position.x + radius/2.f;
            RenderablePoints[2].position.y = position.y + radius/2.f;
            RenderablePoints[2].position.z = position.z;

            RenderablePoints[3].position.x = position.x - radius/2.f;
            RenderablePoints[3].position.y = position.y + radius/2.f;
            RenderablePoints[3].position.z = position.z;

            for (int i=0; i < 4; i++)
            {
                RenderablePoints[i].color = baseColor;
            }

            
            
        }
        
        RenderablePoint RenderablePoints[4]; 

    protected:

        float radius;
        float transparency;
        glm::vec3 position;
        glm::vec4 baseColor;
        std::vector<glm::vec4> litColors;

};

