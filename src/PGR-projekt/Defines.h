#pragma once

#define PARTICLES_IN_CLOUD 60

#define PARTICLE_STRUCT_SIZE 64

#define ANGLE_DIFF_BETWEEN_PARTICLE_UPDATE 20

#define SHADE_RESOLUTION 32

#define LENGTH(A)		(sqrtf(A.x*A.x+A.y*A.y+A.z*A.z))
#define EPSILON			0.0001f
#define TRACEDEPTH                       32
#define INTERSECTION_RES_HIT_INSIDE     -1
#define INTERSECTION_RES_HIT_OUTSIDE     1
#define INTERSECTION_RES_MISS            0
#define GRIDSIZE                         32 //

#define USE_GRID






#define PI (4.0f * (float) atan(1.0f))


#define RAND_MODULO 20000
#define DEFAULT_PERLIN_NOISE_OCTAVES 6
#define GENERATING_TEXTURE_INFO 0
#define DEFAULT_PERLIN_PERSISTANCE 1.0/sqrt(2.0f)
#define DEFAULT_PERLIN_ZOOM  80 //200
#define DEFAULT_PERLIN_SHARPENESS 0.5f
#define DEFAULT_PERLIN_TEXTURE_COVER 128
