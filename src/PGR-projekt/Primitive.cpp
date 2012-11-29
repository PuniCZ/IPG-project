#include "Primitive.h"
#include <iostream>

Primitive::Primitive(void)
    :isLight(false)
{
}


Primitive::~Primitive(void)
{
}


int Plane::Intersect(Ray& ray, float& dist)
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
                return INTERSECTION_RES_HIT_OUTSIDE;
            }
        }
    }

    return INTERSECTION_RES_MISS;
}

bool Plane::IntersectBox(GridBox& box)
{
    glm::vec3 v[2];
    v[0] = box.GetPos(), v[1] = box.GetPos() + box.GetSize();
    int side1, side2, i;
    for (side1 = 0, side2 = 0, i = 0; i < 8; i++ )
    {
        glm::vec3 p( v[i & 1].x, v[(i >> 1) & 1].y, v[(i >> 2) & 1].z );
        if ((glm::dot( p, normal ) + distance) < 0) 
            side1++; 
        else 
            side2++;
    }
    return !((side1 == 0) || (side2 == 0));
}

GridBox Plane::GetBoundingBox()
{
    return GridBox(glm::vec3(-10000), glm::vec3(20000));
}


int Sphere::Intersect(Ray& ray, float& dist)
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
                    return INTERSECTION_RES_HIT_OUTSIDE; //inside
                }
            }
            else
            {
                if (i1 < dist)
                {
                    dist = i1;
                    return INTERSECTION_RES_HIT_INSIDE;
                }
            }
        }
    }
    return INTERSECTION_RES_MISS;
}

bool Sphere::IntersectBox(GridBox& box)
{
    float dmin = 0;
    glm::vec3 v1 = box.GetPos(), v2 = box.GetPos() + box.GetSize();
    if (position.x < v1.x) 
    {
        dmin = dmin + (position.x - v1.x) * (position.x - v1.x);
    }
    else if (position.x > v2.x)
    {
        dmin = dmin + (position.x - v2.x) * (position.x - v2.x);
    }
    if (position.y < v1.y)
    {
        dmin = dmin + (position.y - v1.y) * (position.y - v1.y);
    }
    else if (position.y > v2.y)
    {
        dmin = dmin + (position.y - v2.y) * (position.y - v2.y);
    }
    if (position.z < v1.z)
    {
        dmin = dmin + (position.z - v1.z) * (position.z - v1.z);
    }
    else if (position.z > v2.z)
    {
        dmin = dmin + (position.z - v2.z) * (position.z - v2.z);
    }
    return (dmin <= radius);
}

GridBox Sphere::GetBoundingBox()
{
    return GridBox(position - glm::vec3(radius), glm::vec3(radius) * 2.f);
}


int Particle::Intersect(Ray& ray, float& dist)
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
            
            float distToCenter = LENGTH(glm::vec3(contactPoint - this->position));
            if (distToCenter < this->radius)
            {
                dist = glm::dot(normal, contactPoint - ray.GetOrigin());
                //float c = 1-(((float)rand()/RAND_MAX/distToCenter/20)*radius);
                //float c = 1 - distToCenter/radius;

                glm::vec3 planePos(contactPoint - this->position);
                int texPosX = glm::clamp((int)(planePos.x / radius * 255) + 128, 0, 255);
                int texPosY = glm::clamp((int)(planePos.y / radius * 255) + 128, 0, 255);

                

                float c = this->texture[(texPosY * 255 + texPosX)]/255.f;
                //std::cout << texPosX << " " << texPosY << " " << c << "\n"; 

                material.SetColor(glm::vec4(c, c, c, 1));

                return INTERSECTION_RES_HIT_OUTSIDE;
            }

        }        
    }

    return INTERSECTION_RES_MISS;
}

bool Particle::IntersectBox(GridBox& box)
{
    //using sphere aproximation
    float dmin = 0;
    glm::vec3 v1 = box.GetPos(), v2 = box.GetPos() + box.GetSize();
    if (position.x < v1.x) 
    {
        dmin = dmin + (position.x - v1.x) * (position.x - v1.x);
    }
    else if (position.x > v2.x)
    {
        dmin = dmin + (position.x - v2.x) * (position.x - v2.x);
    }
    if (position.y < v1.y)
    {
        dmin = dmin + (position.y - v1.y) * (position.y - v1.y);
    }
    else if (position.y > v2.y)
    {
        dmin = dmin + (position.y - v2.y) * (position.y - v2.y);
    }
    if (position.z < v1.z)
    {
        dmin = dmin + (position.z - v1.z) * (position.z - v1.z);
    }
    else if (position.z > v2.z)
    {
        dmin = dmin + (position.z - v2.z) * (position.z - v2.z);
    }
    return (dmin <= radius);
}

GridBox Particle::GetBoundingBox()
{
    //using sphere aproximation
    return GridBox(position - glm::vec3(radius), glm::vec3(radius) * 2.f);
}
