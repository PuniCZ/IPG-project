#pragma once

#include "pgr.h"
#include "CloudParticle.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class Cloud
{
    public:
        Cloud(glm::vec3 position, glm::vec3 size);
        ~Cloud(void);

        void CopyParticlesToBuffer(GLuint VBO, int offset);

        int GetNumberOfParticles() { return particles.size(); }

        static const int particlesInCloud = 3000;

    protected:
        std::vector<CloudParticle> particles;
};

