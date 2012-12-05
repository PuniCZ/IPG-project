#pragma once

#include "CameraPlane.h"
#include "Ray.h"
#include "Scene.h"
#include "Defines.h"

class RaytracerResult
{
public:
    RaytracerResult(Primitive* prim, int refl, int lighs, float distance)
        :primitive(prim), numOfReflectedPrimitoves(refl), numOfVisibleLights(lighs), distance(distance)
    { }

     RaytracerResult()
         :primitive(NULL), numOfReflectedPrimitoves(0), numOfVisibleLights(0), distance(0.f)
    { }

    Primitive* primitive;
    int numOfReflectedPrimitoves;
    int numOfVisibleLights;
    float distance;

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
    Raytracer(CameraPlane* camera, Scene& scene);
    ~Raytracer(void);

    bool Render();
    void Init();

    RaytracerResult Raytrace(Ray& ray, glm::vec4&color, int depth, float& distance, float refractionIndex);
    int getCurrentLine(){return curLine;};


    int FindNearest(Ray& ray, float& dist, Primitive*& primitive);
    RaytracerResult Raytracer::RenderRay(glm::vec3& screenPos, glm::vec4& color);

private:

    void applyFog(glm::vec4& color, float distance);
    void applyFarFilter(glm::vec4& color, float distance);
    void applyFarFilter(glm::vec4& color, float distance, glm::vec4& srcColor);

    CameraPlane* camera;
    Scene& scene;

    //rendering
    float screenX1, screenX2, screenY1, screenY2;
    float screenDiffX, screenDiffY;
    float posX, posY;
    int curLine;

    //grid stepping
    glm::vec3 cellSizeRev, cellSize;
};

