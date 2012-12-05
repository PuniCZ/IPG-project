#include "Sky.h"
#include <iostream>
using namespace std;
#include "Utils.h"


using namespace std;


Sky::Sky(int numOfClouds)
{
    srand(time(0));
    ix=(unsigned long) rand();
    for (int i = 0; i < numOfClouds; i++)
    {
        //Viditelne souradnice pro prislusne Z
        /***************************************
        * Z       X         Y
        * 5     -8 - 8     2 - 6
        * 10   -12 - 12    4 - 8
        * 15   -16 - 16    6 - 10
        * 20   -20 - 20    8 - 12
        * 25   -24 - 24   10 - 14
        ****************************/
        
        /*float zPos=10.5f + Utils::Gaussrand(8.f,10.5f)+5;
        float xPos=Utils::Gaussrand(4.f+(0.8f*zPos),4.f + (0.8f*zPos));
        float yPos=fabs(Utils::Gaussrand(3.f,4.f))+zPos*0.4f;*/

        float zPos=10.5f + Gaussrand(8.f,10.5f)+5;
        float xPos=Gaussrand(4.f+(0.8f*zPos),4.f + (0.8f*zPos));
        float yPos=fabs(Gaussrand(3.f,4.f))+zPos*0.4f;
        //cout<<zPos<<" "<<xPos<<" "<<yPos<<endl;
        
        /*float xSizeMult=1+fabs(Utils::Gaussrand(1.1f*zPos,1.1f*zPos));
        float ySizeMult=1+fabs(Utils::Gaussrand(1.1f*zPos,1.1f*zPos));
        float zSizeMult=1+fabs(Utils::Gaussrand(1.1f*zPos,1.1f*zPos));

        float xSize = 1.5f, ySize=0.5f, zSize=0.2f;
		*/
		 float xSizeMult=0.5+Random();
        float ySizeMult=0.7+Random();
        float zSizeMult=0.7+Random();

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


float Sky::Gaussrand(float deviation, float clamp)
{
    static float V1, V2, S;
    static int phase = 0;
    float X;

    if(phase == 0) {
        do {
            float U1 = (float)Random()*12453 / RAND_MAX;
            float U2 = (float)Random()*35123 / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
            } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;
    return glm::clamp(X * deviation, -clamp, clamp);
}

float Sky::Random(void) 
{
    ix = ix * 69069L + 1; // implicitní operace modulo
    return ix / ((double)ULONG_MAX + 1);
}