#include "Cloud.h"
#include <time.h>

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
        spere->SetTexture(Texture(127,127,true));
        spere->GetTexture()->setSmoothNoise(false);
        spere->GetTexture()->setZoom(150);
        spere->GetTexture()->generateTexture();
        scene.GetPrimitives()->push_back(spere);
    }
}

