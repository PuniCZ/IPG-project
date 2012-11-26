#pragma once

#include "pgr.h"
#include "CloudParticle.h"
#include "Light.h"
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
        void SortParticles(const glm::vec3& viewDir, const glm::vec3& vecSortPoint, bool dirToward);

        

        class ParticleAwayComparator
        {
            public:
                bool operator()(const CloudParticle& A, const CloudParticle& B)
                {
                    return (A < B);
                }
        };
  
        class ParticleTowardComparator
        {
            public:
                bool operator()(const CloudParticle& A, const CloudParticle& B)
                {
                    return (A > B);
                }
        };

        bool Update(glm::vec3 newViewPos);

        void Illuminate(Light& light, bool isFirstLight);

    protected:
        std::vector<CloudParticle> particles;

        std::vector<glm::vec3> lightDirections;

        float radius;

        glm::vec3 centerPosition;

        // particle sorting functors for STL sort.
        ParticleTowardComparator towardComparator; 
        ParticleAwayComparator awayComparator;

        glm::vec3 lastViewPos;

        bool needsToBeUpdated(glm::vec3 newViewPos)
        {
            return glm::degrees(acos(glm::dot(newViewPos, lastViewPos))) > ANGLE_DIFF_BETWEEN_PARTICLE_UPDATE;
        }

        
};

