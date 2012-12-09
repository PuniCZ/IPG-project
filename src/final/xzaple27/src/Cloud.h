#pragma once

#include "CloudParticle.h"
#include "Scene.h"
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Cloud
{
    public:
        Cloud(glm::vec3 position, glm::vec3 size);
        ~Cloud(void);

        void CopyParticlesToScene(Scene& scene);

    protected:

        std::vector<CloudParticle> particles;

        float radius;
        glm::vec3 centerPosition;
        Texture tex;

        
};

