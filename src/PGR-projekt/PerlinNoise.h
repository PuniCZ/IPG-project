#pragma once
class PerlinNoise
{
public:
	PerlinNoise(void);
	~PerlinNoise(void);

private:
	double noise(int x, int y);
	double smoothNoise(int x, int y);
	double interpolate(double a, double b, double x);

};

