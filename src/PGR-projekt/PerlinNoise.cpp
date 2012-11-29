#include "PerlinNoise.h"
#include <cmath>

PerlinNoise::PerlinNoise(void)
{
}


PerlinNoise::~PerlinNoise(void)
{
}

double PerlinNoise::noise(int x, int y)
{
	int n=x+y*57;
	n=(n<<13)^n; //this is xor not pow
	return  (1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);   
}

double PerlinNoise::smoothNoise(int x, int y)
{
	double corners = (noise(x-1,y-1) + noise(x+1,y-1) + noise(x-1,y+1) + noise(x+1,y+1))/16.0;
	double sides = (noise(x-1,y) + noise(x+1,y) + noise(x,y+1) + noise(x,y+1))/8.0;
	double center = noise(x,y)/4.0;
	return corners + sides + center;
}

double PerlinNoise::interpolate(double a, double b, double x)
{
	double ft=x * 3.1415927;
	double f=(1.0-cos(ft))* 0.5;
	return a*(1.0-f)+b*f;
}