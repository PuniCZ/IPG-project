#include "CameraPlane.h"


CameraPlane::CameraPlane(glm::vec3 position, glm::vec3 direction, glm::vec2 resolution)
    :origin(origin), direction(glm::normalize(direction)), buffer(resolution.x, resolution.y)
{
    
}


CameraPlane::~CameraPlane(void)
{
}
