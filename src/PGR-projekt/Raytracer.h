#pragma once

#include "CameraPlane.h"
#include "Ray.h"
#include "Scene.h"
#include "Defines.h"

class RaytracerResult
{
public:
    RaytracerResult(Primitive* prim, int refl, int lighs)
        :primitive(prim), numOfReflectedPrimitoves(refl), numOfVisibleLights(lighs)
    { }

     RaytracerResult()
        :primitive(NULL), numOfReflectedPrimitoves(0), numOfVisibleLights(0)
    { }

    Primitive* primitive;
    int numOfReflectedPrimitoves;
    int numOfVisibleLights;

    bool operator==(const RaytracerResult& another)
	{
        return 
            this->numOfReflectedPrimitoves == another.numOfReflectedPrimitoves && 
            this->numOfVisibleLights == another.numOfVisibleLights && 
            this->primitive == another.primitive;
	}

    bool operator!=(const RaytracerResult& another)
	{
        return !(this==&another);
	}
};


class Raytracer
{
public:
    Raytracer(CameraPlane* camera);
    ~Raytracer(void);

    bool Render(Scene& scene);

    RaytracerResult Raytrace(Scene& scene, Ray& ray, glm::vec4&color, int depth, float& distance, float refractionIndex);
	int getCurrentLine(){return curLine;};

private:
    CameraPlane* camera;

    float screenX1, screenX2, screenY1, screenY2;
    float screenDiffX, screenDiffY;

    float posX, posY;

    int curLine;

};

