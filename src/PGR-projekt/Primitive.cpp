#include "Primitive.h"


Primitive::Primitive(void)
    :isLight(false)
{
}


Primitive::~Primitive(void)
{
}


bool Plane::Intersect(Ray& ray, float& dist)
{
    float dot = glm::dot(this->normal, ray.GetDirection());
    if (dot != 0) //not parallel
    {
        float tmpDist = -(glm::dot(this->normal, ray.GetOrigin()) + this->distance) / dot;
        if (tmpDist > 0) //possible hit
        {
            if (tmpDist < dist) //hit and closer than dist
            {
                dist = tmpDist;
                return true;
            }
        }
    }

	return false;
}


bool Sphere::Intersect(Ray& ray, float& dist)
{    
    glm::vec3 v = ray.GetOrigin() - this->position;
    float b = -glm::dot(v, ray.GetDirection());
	float det = (b * b) - glm::dot(v, v) + this->sqrRadius;
	bool retval = false;
	if (det > 0)
	{
		det = sqrtf(det);
		float i1 = b - det;
		float i2 = b + det;
		if (i2 > 0)
		{
			if (i1 < 0) 
			{
				if (i2 < dist) 
				{
					dist = i2;
					return true; //inside
				}
			}
			else
			{
				if (i1 < dist)
				{
					dist = i1;
					return true;
				}
			}
		}
	}
	return false;
}


bool Particle::Intersect(Ray& ray, float& dist)
{
    //glm::vec3 normal(glm::normalize(ray.GetOrigin() - this->position)); 

    float dotNormalDirection = glm::dot(normal, ray.GetDirection());
    float dotNormalOrigin = glm::dot(normal, ray.GetOrigin());
    float dotNormalPosition = glm::dot(normal, this->position);
    if (dotNormalDirection != 0) //not parallel
    {
        // Compute the t value for the directed ray intersecting the plane
        float t = (dotNormalPosition - dotNormalOrigin) / dotNormalDirection;

        //ve have intersection
        if (t >= 0.f/* && t <= 1.0f*/)
        {
            // scale the ray by t
            glm::vec3 contactPoint(ray.GetOrigin() + (ray.GetDirection() * t));
            
            float distToCenter = sqrtf(glm::dot(contactPoint - this->position, contactPoint - this->position));

            if (distToCenter < this->radius)
            {
                dist = glm::dot(normal, contactPoint - ray.GetOrigin());
                //float c = 1-(((float)rand()/RAND_MAX/distToCenter/20)*radius);
                float c = 1 - distToCenter/radius;
                material.SetColor(glm::vec4(c, c, c, c));

                return true;
            }

        }        
    }

	return false;
}
