#pragma once
#include <vector>
#include "glm/glm.hpp"

class ScreenBuffer
{
public:
    ScreenBuffer(int width, int height);
    ~ScreenBuffer(void);

    void SetPixel(int x, int y, glm::vec3 color)
    {
        pixels[width * y + x] = color;
    }

    void SetPixel(int x, int y, glm::vec4 color)
    {
        pixels[width * y + x] = glm::vec3(color.r, color.g, color.b);
    }

    glm::vec3 GetPixel(int x, int y) const 
    {
        return pixels[width * y + x];
    }

private:
    int width;
    int height;
    std::vector<glm::vec3> pixels;
};

