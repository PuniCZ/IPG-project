#include "Sky.h"

Sky::Sky(int numOfClouds)
{

    srand((unsigned int)time(NULL));
    for (int i = 0; i < numOfClouds; i++)
    {
        /***************************************
        * Z       X         Y
        * 5     -8 - 8     2 - 6
        * 10   -12 - 12    4 - 8
        * 15   -16 - 16    6 - 10
        * 20   -20 - 20    8 - 12
        * 25   -24 - 24   10 - 14
        * 30   -28 - 28   12 - 16
        ****************************/
        // TODO:PREDELAT
        double zPos=(rand()%5)*5+5;
        double xPos=(rand()%(4*int(zPos/5)+4)*2)-20;
        double yPos=(rand()%5)+int(zPos/5)*2;
        //double xPos = rand()%30-15, yPos = rand()%-15, zPos  = rand()%20+5;
        double xSize = 1.5f, ySize=0.5f, zSize=0.2f;

        //double xPos=3+Utils::Gaussrand(8,0);
        //double yPos=3+Utils::Gaussrand(3,5);
        //double zPos=3+Utils::Gaussrand(2,4);


        clouds.push_back(Cloud(glm::vec3(xPos, yPos, zPos), glm::vec3(xSize,ySize,zSize)));
    }
};

void Sky::CopySkyToScene(Scene& scene)
{
    for (std::vector<Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++)
    {
        it->CopyParticlesToScene(scene);
    }
}

