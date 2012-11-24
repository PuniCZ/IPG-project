#include "Cloud.h"
#include <time.h>

Cloud::Cloud(void)
{
    srand(time(NULL));
    for (int i = 0; i < 2000; i++)
    {
        particles.push_back(CloudParticle(i*.2f));
    }
}


Cloud::~Cloud(void)
{
}


void Cloud::CopyParticlesToBuffer(GLuint VBO, GLuint EBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //allocate buffer memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*28*particles.size(), NULL, GL_STREAM_DRAW);
    //fill buffer
    float *buffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    int bufferOffset = 0;    
    for (std::vector<CloudParticle>::iterator it = particles.begin(); it != particles.end(); it++)
    {
        memcpy(buffer + bufferOffset + 0, glm::value_ptr(it->RenderablePoints[0].color), sizeof(float)*4);
        memcpy(buffer + bufferOffset + 4, glm::value_ptr(it->RenderablePoints[0].position), sizeof(float)*3);

        memcpy(buffer + bufferOffset + 7, glm::value_ptr(it->RenderablePoints[1].color), sizeof(float)*4);
        memcpy(buffer + bufferOffset + 11, glm::value_ptr(it->RenderablePoints[1].position), sizeof(float)*3);

        memcpy(buffer + bufferOffset + 14, glm::value_ptr(it->RenderablePoints[2].color), sizeof(float)*4);
        memcpy(buffer + bufferOffset + 18, glm::value_ptr(it->RenderablePoints[2].position), sizeof(float)*3);

        memcpy(buffer + bufferOffset + 21, glm::value_ptr(it->RenderablePoints[3].color), sizeof(float)*4);
        memcpy(buffer + bufferOffset + 25, glm::value_ptr(it->RenderablePoints[3].position), sizeof(float)*3);

        bufferOffset += 28;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //allocate buffer memory
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*6*particles.size(), NULL, GL_STREAM_DRAW);
    //fill buffer
    unsigned int *buffer2 = (unsigned int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);    
    int offset1 = 0;
    int offset2 = 0;
    for (std::vector<CloudParticle>::iterator it = particles.begin(); it != particles.end(); it++)
    {
        buffer2[offset1 + 0] = offset2 + 0;
        buffer2[offset1 + 1] = offset2 + 1;
        buffer2[offset1 + 2] = offset2 + 2;

        buffer2[offset1 + 3] = offset2 + 0;
        buffer2[offset1 + 4] = offset2 + 2;
        buffer2[offset1 + 5] = offset2 + 3;

        offset1 += 6;
        offset2 += 4;
    }
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);


}