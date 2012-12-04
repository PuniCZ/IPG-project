#include "Cloud.h"
#include <time.h>
#include <algorithm>
#include <glm/gtx/vector_angle.hpp>

Cloud::Cloud(glm::vec3 position, glm::vec3 size)
    :centerPosition(position), radius(std::max(std::max(size.x, size.y), size.z))
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < PARTICLES_IN_CLOUD; i++)
    {
        particles.push_back(CloudParticle(position, size));
    }
}


Cloud::~Cloud(void)
{
}


void Cloud::CopyParticlesToScene(Scene& scene)
{    
    for (std::vector<CloudParticle>::iterator it = particles.begin(); it != particles.end(); it++)
    {
        Sphere* spere = new Sphere(it->GetPosition(), it->GetSize());
        spere->SetMaterial(Material(glm::vec4(.7f, .7f, .7f, 0.5f), 1.f, 0.0f, .8f, 1.f));
        spere->SetTexture(Texture(255,255,true));
        spere->GetTexture()->setSmoothNoise(true);
        spere->GetTexture()->generateTexture();
        scene.GetPrimitives()->push_back(spere);
    }
}

