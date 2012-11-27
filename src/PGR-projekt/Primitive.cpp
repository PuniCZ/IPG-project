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
            if (tmpDist < dist) //hit and coser than dist
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