#pragma once

#include <stdlib.h>
#include <math.h>
#include <glm/glm.hpp>
#include "pgr.h"

#define RAND(max) (float(rand())/RAND_MAX)*max

class Utils
{
    public:

        static float Gaussrand(float deviation, float clamp);
        static inline float EvalHermite(float pA, float pB, float vA, float vB, float u);
        static unsigned char* CreateGaussianMap(int N);
        static GLuint CreateSplatTexture(unsigned int size);

        
};

