#include "Sky.h"
#include "Utils.h"

using namespace std;


Sky::Sky(int numOfClouds)
{
    

    for (int i = 0; i < numOfClouds; i++)
    {
        printf("Generuji mrak %d...\n", i);

        //Viditelne souradnice pro prislusne Z
        /***************************************
        * Z       X         Y
        * 5     -8 - 8     2 - 6
        * 10   -12 - 12    4 - 8
        * 15   -16 - 16    6 - 10
        * 20   -20 - 20    8 - 12
        * 25   -24 - 24   10 - 14
        ****************************/

        float zPos=10.5f + Utils::Gaussrand(8.f,10.5f)+5;
        float xPos=Utils::Gaussrand(3.f+(0.8f*zPos),4.f + (0.8f*zPos));
        float yPos=fabs(Utils::Gaussrand(3.f,4.f))+zPos*0.2f - 2.f;
        
        float xSizeMult=0.5+Utils::Random();
        float ySizeMult=0.7+Utils::Random();
        float zSizeMult=0.7+Utils::Random();

        float xSize = 1.5f * xSizeMult, ySize=0.5f * ySizeMult, zSize=0.2f * zSizeMult;

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



