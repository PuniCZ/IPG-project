#pragma once
#include "Defines.h"
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

class PerlinNoise
{
public:
	PerlinNoise(void)
        : curve(false), smooth(false) , texCover(DEFAULT_PERLIN_TEXTURE_COVER), texSharpness(DEFAULT_PERLIN_SHARPENESS), zoom(DEFAULT_PERLIN_ZOOM), persistance(DEFAULT_PERLIN_PERSISTANCE),
        octaves(DEFAULT_PERLIN_NOISE_OCTAVES)
    {}

    ~PerlinNoise(void) { }
    void setExpCurve(bool curve) { this->curve = curve;}
    void setExpCurve(bool curve, int texCover, double texSharpness) { this->curve = curve; this->texCover = texCover; this->texSharpness = texSharpness; }
    void setExpCurve(int texCover, double texSharpness) { this->texCover = texCover; this->texSharpness = texSharpness; }
    
    void setSmoothNoise(bool smooth) { this->smooth = smooth; }

    void setZoom(int zoom){ this->zoom = zoom;}
    void setPersistance(double persistance){ this->persistance = persistance;}
    void setZoomPersistance(int zoom, double persistance){ this->zoom = zoom, this->persistance = persistance;}
    void setOctaves(int octaves){ this->octaves = octaves; }

    unsigned char *generate(int w,int h);

private:
	double getNoise(int x, int y);
	double getSmoothNoise(int x, int y);
	double interpolate(double a, double b, double x);
	double noise(double x,double y);
	int cloudExpCurve(int v);

    bool curve;
    bool smooth;
    double persistance;
    double texSharpness;
    int texCover;
    int zoom;
    int octaves;

};

