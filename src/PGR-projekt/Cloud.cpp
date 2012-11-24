#include "Cloud.h"
#include <time.h>

Cloud::Cloud(void)
{
    srand(time(NULL));
    for (int i = 0; i < 5000; i++)
    {
        particles.push_back(CloudParticle(i*.2f));
    }
}


Cloud::~Cloud(void)
{
}


void Cloud::CopyParticlesToBuffer(GLuint VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //allocate buffer memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*36*particles.size(), NULL, GL_STREAM_DRAW);
    //fill buffer
    float *buffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    int bufferOffset = 0;    
    for (std::vector<CloudParticle>::iterator it = particles.begin(); it != particles.end(); it++)
    {
        for (int i = 0; i < 4; i++)
        {
            memcpy(buffer + bufferOffset + 0, glm::value_ptr(it->RenderablePoints[i].color), sizeof(float)*4);
            memcpy(buffer + bufferOffset + 4, glm::value_ptr(it->RenderablePoints[i].position), sizeof(float)*3);
            memcpy(buffer + bufferOffset + 7, glm::value_ptr(it->RenderablePoints[i].textCoord), sizeof(float)*2);
            bufferOffset += 9;
        }
        /*memcpy(buffer + bufferOffset + 0, glm::value_ptr(it->RenderablePoints[0].color), sizeof(float)*4);
        memcpy(buffer + bufferOffset + 4, glm::value_ptr(it->RenderablePoints[0].position), sizeof(float)*3);
        memcpy(buffer + bufferOffset + 3, glm::value_ptr(it->RenderablePoints[0].textCoord), sizeof(float)*2);

        memcpy(buffer + bufferOffset + 7, glm::value_ptr(it->RenderablePoints[1].color), sizeof(float)*4);
        memcpy(buffer + bufferOffset + 11, glm::value_ptr(it->RenderablePoints[1].position), sizeof(float)*3);

        memcpy(buffer + bufferOffset + 14, glm::value_ptr(it->RenderablePoints[2].color), sizeof(float)*4);
        memcpy(buffer + bufferOffset + 18, glm::value_ptr(it->RenderablePoints[2].position), sizeof(float)*3);

        memcpy(buffer + bufferOffset + 21, glm::value_ptr(it->RenderablePoints[3].color), sizeof(float)*4);
        memcpy(buffer + bufferOffset + 25, glm::value_ptr(it->RenderablePoints[3].position), sizeof(float)*3);*/
        

        
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}