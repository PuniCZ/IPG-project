#pragma once

#include "CameraPlane.h"
#include "Ray.h"
#include "Scene.h"
#include "Defines.h"

class Raytracer
{
public:
    Raytracer(CameraPlane* camera);
    ~Raytracer(void);

    bool Render(Scene& scene);

    glm::vec4 Raytrace(Scene& scene, Ray& ray, glm::vec4&color, int depth, float& distance);
	int getCurrentLine(){return curLine;};

private:
    CameraPlane* camera;

    float screenX1, screenX2, screenY1, screenY2;
    float screenDiffX, screenDiffY;

    float posX, posY;

    int curLine;

};

