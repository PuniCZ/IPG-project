#pragma once

#include <stdlib.h>
#include <math.h>
#include "glm/glm.hpp"

#define RAND(max) (float(rand())/RAND_MAX)*max

class Utils
{
    public:

        static float Gaussrand(float deviation, float clamp);
        static float Random(void);
        static unsigned long ix;
};


