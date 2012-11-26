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
            float particleSize = std::max(std::max(size.x,size.y),size.z)/5;

            radius = particleSize*2+abs(Utils::Gaussrand(particleSize, particleSize));
            
            position.x += Utils::Gaussrand(size.x, size.x*3);
            position.y += Utils::Gaussrand(size.y, size.y*3);
            position.z += Utils::Gaussrand(size.z, size.z*3);
                        

            Update(baseColor);
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

        void Update(glm::vec4 color)
        {            
            //front
            RenderablePoints[0].position.x = position.x - radius/2.f;
            RenderablePoints[0].position.y = position.y - radius/2.f;
            RenderablePoints[0].position.z = position.z;
            RenderablePoints[0].textCoord.x = 0.0f;
            RenderablePoints[0].textCoord.y = 0.0f;

            RenderablePoints[1].position.x = position.x + radius/2.f;
            RenderablePoints[1].position.y = position.y - radius/2.f;
            RenderablePoints[1].position.z = position.z;
            RenderablePoints[1].textCoord.x = 1.0f;
            RenderablePoints[1].textCoord.y = 0.0f;

            RenderablePoints[2].position.x = position.x + radius/2.f;
            RenderablePoints[2].position.y = position.y + radius/2.f;
            RenderablePoints[2].position.z = position.z;
            RenderablePoints[2].textCoord.x = 1.0f;
            RenderablePoints[2].textCoord.y = 1.0f;

            RenderablePoints[3].position.x = position.x - radius/2.f;
            RenderablePoints[3].position.y = position.y + radius/2.f;
            RenderablePoints[3].position.z = position.z;
            RenderablePoints[3].textCoord.x = 0.0f;
            RenderablePoints[3].textCoord.y = 1.0f;

            for (int i=0; i < 4; i++)
            {
                RenderablePoints[i].color = color;                
            }

            
            
        }

        void SetSquareSortDistance(float squareDistance) { squareSortDistance = squareDistance; }
        float GetSquareSortDistance() { return squareSortDistance; }

        //! This operator is used to sort particle arrays from nearest to farthes.
        bool operator<(const CloudParticle& p) const
        {
            return (squareSortDistance < p.squareSortDistance);
        }

        //! This operator is used to sort particle arrays from farthest to nearest.
        bool operator>(const CloudParticle& p) const
        {
            return (squareSortDistance > p.squareSortDistance);
        }
        
        RenderablePoint RenderablePoints[4];

        void SetBaseColor(glm::vec4 color) { baseColor = color; }
        void ClearLitColors() { litColors.clear(); }
        void AddLitColor(glm::vec4 color) { litColors.push_back(color); }

        //Assumes all atribs are ready
        void Draw()
        {
            glBindBuffer(GL_ARRAY_BUFFER, Utils::glVBO1);
            //allocate buffer memory
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * PARTICLE_STRUCT_SIZE, NULL, GL_STREAM_DRAW);
            //fill buffer
            int bufferOffset = 0; 
            float *buffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            for (int i = 0; i < 4; i++)
            {
                memcpy(buffer + bufferOffset + 0, glm::value_ptr(RenderablePoints[i].color), sizeof(float)*4);
                memcpy(buffer + bufferOffset + 4, glm::value_ptr(RenderablePoints[i].position), sizeof(float)*3);
                memcpy(buffer + bufferOffset + 7, glm::value_ptr(RenderablePoints[i].textCoord), sizeof(float)*2);
                bufferOffset += 9; //Important: if changed, change also PARTICLE_STRUCT_SIZE
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
            //draw contents
            //glDisableVertexAttribArray(normalAttrib);
            //glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, NULL);
            //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);//draw shading quad
            glDrawArrays(GL_QUADS, 0, 4);
            //glEnableVertexAttribArray(normalAttrib);    
        }


    protected:

        float radius;
        float transparency;
        float squareSortDistance;
        glm::vec3 position;
        glm::vec4 baseColor;
        std::vector<glm::vec4> litColors;

};

