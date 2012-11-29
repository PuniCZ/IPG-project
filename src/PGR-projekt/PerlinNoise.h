#pragma once
class PerlinNoise
{
public:
	PerlinNoise(void);
	~PerlinNoise(void);
	unsigned char *generate(int w,int h,double zoom,double p);

private:
	double getNoise(int x, int y);
	double smoothNoise(int x, int y);
	double interpolate(double a, double b, double x);
	double noise(double x,double y);
	
};

