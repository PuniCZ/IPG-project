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
        Cloud(void);
        ~Cloud(void);

        void CopyParticlesToBuffer(GLuint VBO);

        int GetNumberOfParticles() { return particles.size(); }

    protected:
        std::vector<CloudParticle> particles;
};

