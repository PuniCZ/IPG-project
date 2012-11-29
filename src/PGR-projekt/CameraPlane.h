#pragma once

#include <glm\glm.hpp>
#include "ScreenBuffer.h"

class CameraPlane
{
public:
    CameraPlane(glm::vec3 origin, glm::vec3 direction, glm::vec2 resolution);
    ~CameraPlane(void);

    void SetResolution(int width, int height)
    {
        SetResolution(glm::vec2(width, height));
    }

    void SetResolution(glm::vec2 resolution) 
    { 
        this->resolution = resolution; 
        buffer = ScreenBuffer(resolution.x, resolution.y);
    }

    float GetWidth() { return resolution.x; }

    float GetHeight() { return resolution.y; }

    glm::vec3 GetOrigin() { return origin; }

    ScreenBuffer* GetBuffer()
    {
        return &buffer;
    }

    glm::vec4 GetBgColor() { return bgColor; }
    void SetBgColor(glm::vec4 color) 
    {
        bgColor = color;
    }

    bool GetUseSuperSampling() { return useSuperSampling; }
    void SetUseSuperSampling(bool use)
    {
        useSuperSampling = use;
    }


protected:
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec2 resolution;

    ScreenBuffer buffer;

    glm::vec4 bgColor;

    bool useSuperSampling;
    
};

