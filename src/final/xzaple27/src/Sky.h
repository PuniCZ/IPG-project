#pragma once
#include "Cloud.h"

class Sky
{
public:
    Sky(int numOfClouds);
    ~Sky(void){};
    void CopySkyToScene(Scene& scene);

protected:
    std::vector<Cloud> clouds;
    int cloudsNumber;
    //static float Gaussrand(float deviation, float clamp);
    //float Random(void);
    //unsigned long ix;
};


