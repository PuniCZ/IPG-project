#include "Light.h"


Light::Light(glm::vec3 pos, glm::vec3 dir, glm::vec4 cDiffuse, glm::vec4 cAmbient, glm::vec4 cSpecular)
    : position(pos), direction(dir), colorAmbient(cAmbient), colorDiffuse(cDiffuse), colorSpecular(cSpecular)
{
}


Light::~Light(void)
{
}
