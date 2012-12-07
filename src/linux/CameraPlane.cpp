#include "CameraPlane.h"


CameraPlane::CameraPlane(glm::vec3 origin, glm::vec3 direction, glm::vec2 resolution)
    :origin(origin), direction(glm::normalize(direction)), resolution(resolution),
        buffer((int)resolution.x, (int)resolution.y), bgColor(0.f), useSuperSampling(true)
{
    
}


CameraPlane::~CameraPlane(void)
{
}
