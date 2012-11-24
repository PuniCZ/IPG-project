#include "Cloud.h"
#include <time.h>

Cloud::Cloud(glm::vec3 position, glm::vec3 size)
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < particlesInCloud; i++)
    {
        particles.push_back(CloudParticle(position, size));
    }
}


Cloud::~Cloud(void)
{
}


void Cloud::CopyParticlesToBuffer(GLuint VBO, int offset)
{    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);   
    //fill buffer
    float *buffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    int bufferOffset = offset;    
    for (std::vector<CloudParticle>::iterator it = particles.begin(); it != particles.end(); it++)
    {
        for (int i = 0; i < 4; i++)
        {
            memcpy(buffer + bufferOffset + 0, glm::value_ptr(it->RenderablePoints[i].color), sizeof(float)*4);
            memcpy(buffer + bufferOffset + 4, glm::value_ptr(it->RenderablePoints[i].position), sizeof(float)*3);
            memcpy(buffer + bufferOffset + 7, glm::value_ptr(it->RenderablePoints[i].textCoord), sizeof(float)*2);
            bufferOffset += 9; //Important: if changed, change also allocation
        }        
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}