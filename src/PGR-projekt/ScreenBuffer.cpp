#include "ScreenBuffer.h"


ScreenBuffer::ScreenBuffer(int width, int height)
    :width(width), height(height)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            pixels.push_back(glm::vec3(0, 0, 0));

}


ScreenBuffer::~ScreenBuffer(void)
{
}
