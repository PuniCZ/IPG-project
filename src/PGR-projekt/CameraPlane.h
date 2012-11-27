#pragma once

#include <glm\glm.hpp>
#include "ScreenBuffer.h"

class CameraPlane
{
public:
    CameraPlane(glm::vec3 position, glm::vec3 direction, glm::vec2 resolution);
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

    ScreenBuffer* GetBuffer()
    {
        return &buffer;
    }

protected:
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec2 resolution;

    ScreenBuffer buffer;
    
};

