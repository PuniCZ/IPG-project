#include "Sky.h"

Sky::Sky(int numOfClouds)
{

    srand((unsigned int)time(NULL));
    for (int i = 0; i < numOfClouds; i++)
    {
        clouds.push_back(Cloud(glm::vec3(2, 2, 5), glm::vec3(1.5f,1.f,0.3f)));
    }
};

void Sky::CopySkyToScene(Scene& scene)
{
    for (std::vector<Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++)
    {
        it->CopyParticlesToScene(scene);
    }
}

