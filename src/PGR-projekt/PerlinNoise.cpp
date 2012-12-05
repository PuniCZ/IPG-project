#include "PerlinNoise.h"
#include <cmath>



double PerlinNoise::getNoise(int x, int y)
{
	int n=x+y*57;
	n=(n<<13)^n; //this is xor not pow
	return  (1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);   
}

double PerlinNoise::getSmoothNoise(int x, int y)
{
	double corners = (getNoise(x-1, y-1) + getNoise(x+1, y-1) + getNoise(x-1, y+1) + getNoise(x+1, y+1))/16.0;
	double sides = (getNoise(x-1, y) + getNoise(x+1, y) + getNoise(x, y+1) + getNoise(x, y+1))/8.0;
	double center = getNoise(x, y)/4.0;
	return corners + sides + center;
}

double PerlinNoise::interpolate(double a, double b, double x)
{
	double ft=x * PI;
	double f=(1.0 - cos(ft)) * 0.5;
	return a*(1.0 - f) + b * f;
}

double PerlinNoise::noise(double x,double y)
{
    double modifiedX=floor(x);
    double modifiedY=floor(y);

    double s,t,u,v;
    
    if(this->smooth)
    {
        s = getSmoothNoise(modifiedX,   modifiedY); 
        t = getSmoothNoise(modifiedX+1, modifiedY);
        u = getSmoothNoise(modifiedX,   modifiedY+1);
        v = getSmoothNoise(modifiedX+1, modifiedY+1);
    }
    else
    {
        s = getNoise(modifiedX,   modifiedY); 
        t = getNoise(modifiedX+1, modifiedY);
        u = getNoise(modifiedX,   modifiedY+1);
        v = getNoise(modifiedX+1, modifiedY+1);
    }
    double int1 = interpolate(s, t, x-modifiedX);
    double int2 = interpolate(u, v, x-modifiedX);
    
    return interpolate(int1, int2, y-modifiedY);
}

int PerlinNoise::cloudExpCurve(int v)
{
	int c=v-this->texCover;
	if(c < 0)
		c = 0;

	return (255 - (pow(this->texSharpness, c) * 255));
}


unsigned char *PerlinNoise::generate(int width, int height)
{
    #if GENERATING_TEXTURE_INFO
        std::cout<<"Generating texture...";
    #endif

    unsigned char *nt = new unsigned char[width*height];

    //std::srand(time(NULL));
    int r1 = std::rand() % RAND_MODULO;
    int r2 = std::rand() % RAND_MODULO;

    for(int yy=r1, y=0; yy<(height + r1) ;yy++,y++)
    {
        for(int xx=r2, x=0; xx<(width + r2); xx++,x++)
        {
            double sumNoise = 0;
            for(int i=0; i< this->octaves-1 ;i++)
            {
                double frequency = pow(2.0,i);
                double amplitude = pow(this->persistance,i);    
                sumNoise += noise(((double)xx)*frequency/this->zoom, ((double)yy)/this->zoom*frequency)*amplitude;
            }										
            
            int color = (int)((sumNoise * 128.0) + 128.0);
            
            if(this->curve)
                color = cloudExpCurve(color);
            
            if(color > 255)
                color = 255;
            if(color < 0)
                color = 0;

            nt[x*width+y] = color;
        }
    }

    #if GENERATING_TEXTURE_INFO
        std::cout<<" done" <<std::endl;
    #endif

    return nt;
}
