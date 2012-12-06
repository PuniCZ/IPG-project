#include "Primitive.h"
#include <iostream>

Primitive::Primitive(void)
    :isLight(false), rayID(0)
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
    glm::vec3 pos(-10000);
    glm::vec3 size(20000);
    return GridBox(pos, size);
}


int Sphere::Intersect(Ray& ray, float& dist)
{
    float d(dist);
    int retval = INTERSECTION_RES_MISS;
    if (intersect(ray, d))
    {
        glm::vec3 pi(ray.GetOrigin() + ray.GetDirection() * d);
        float density = getTextureDensity(pi) * this->radius;
        float newradius = density;

        glm::vec3 v = ray.GetOrigin() - this->position;
        float b = -glm::dot(v, ray.GetDirection());
        float det = (b * b) - glm::dot(v, v) + newradius*newradius;
        
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
                        retval = INTERSECTION_RES_HIT_INSIDE; //inside
                    }
                }
                else
                {
                    if (i1 < dist)
                    {
                        dist = i1;
                        retval = INTERSECTION_RES_HIT_OUTSIDE;
                    }
                }
            }
        }
    }

    return retval;
}

int Sphere::intersect(Ray& ray, float& dist)
{
    glm::vec3 v = ray.GetOrigin() - this->position;
    float b = -glm::dot(v, ray.GetDirection());
    float det = (b * b) - glm::dot(v, v) + this->sqrRadius;
    int retval = INTERSECTION_RES_MISS;
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
                    retval = INTERSECTION_RES_HIT_INSIDE; //inside
                }
            }
            else
            {
                if (i1 < dist)
                {
                    dist = i1;
                    retval = INTERSECTION_RES_HIT_OUTSIDE;
                }
            }
        }
    }
    return retval;
}

float Sphere::getTextureDensity(glm::vec3& pos)
{
    Texture *t=this->GetTexture();
    if(!t->isEnabled())
        return 1.f;
    else
    {
        glm::vec3 pole(0, 1, 0);
        glm::vec3 texPos(1, 0, 0);
        float Vscale=2;
        float Uscale=1;

        glm::vec3 vp = (pos - this->position) * this->revRadius;
        float phi = acosf( -glm::dot(vp, pole));

        float u=phi * Vscale * (1.0f / PI), v = phi * Vscale * (1.0f / PI);
        float theta = (acosf( glm::dot(texPos, vp ) / sinf(phi))) * (2.0f / PI);
        if (glm::dot( glm::cross(pole, texPos), vp ) >= 0)
            u = (1.0f - theta) * Uscale;
        else
            u = theta * Uscale;

        unsigned char *texture=this->GetTexture()->getTexture();


        int xx=((int)(u * t->getWidth()) + t->getWidth()/2)%t->getWidth();
        int yy=((int)(v * t->getHeight()) + t->getHeight()/2)%t->getHeight();
        if(xx<0)
            xx*=-1;
        if(yy<0)
            yy*=-1;

        return texture[(yy * t->getHeight() + xx)]/255.f;
    }
}

glm::vec4 Sphere::GetColor(glm::vec3& pos, glm::vec3 origin)
{
    glm::vec4 retval(0);
    Texture *t=this->GetTexture();
    if(!t->isEnabled())
        retval = material.GetColor(); 
    else
    {
        //TODO: Use source vector in futere glm::degrees(acos(glm::dot(newViewPos, lastViewPos))) > ANGLE_DIFF_BETWEEN_PARTICLE_UPDATE;
        float mult = glm::clamp(-GetNormal(pos).z, 0.f, 1.f);
        mult = glm::clamp((float)exp(mult)-1.8f, 0.f, 1.f)*1.5f;
        //mult = glm::clamp(-pow(0.8f, mult)+1, 0.f, 1.f)*mult*5;

        if (mult == 0.f)
            return retval;

        glm::vec3 pole(1, 0, 0);
        glm::vec3 texPos(0, 1, 0);
        float Vscale=2;
        float Uscale=1;

        glm::vec3 vp = (pos - this->position) * this->revRadius;
        float phi = acosf( -glm::dot(vp, pole));

        float u=phi * Vscale * (1.0f / PI), v = phi * Vscale * (1.0f / PI);
        float theta = (acosf( glm::dot(texPos, vp ) / sinf(phi))) * (2.0f / PI);
        if (glm::dot( glm::cross(pole, texPos), vp ) >= 0)
            u = (1.0f - theta) * Uscale;
        else
            u = theta * Uscale;

        unsigned char *texture=this->GetTexture()->getTexture();


        int xx=((int)(u * t->getWidth()) + t->getWidth()/2)%t->getWidth();
        int yy=((int)(v * t->getHeight()) + t->getHeight()/2)%t->getHeight();
        if(xx<0)
            xx*=-1;
        if(yy<0)
            yy*=-1;
        float c = texture[(yy * t->getHeight() + xx)]/255.f;
        //std::cout << mult << "\n";
        //retval = glm::vec4(mult, 0,0,1);//(c * material.GetColor()) * mult;
        retval = (c * material.GetColor())* (mult);
    }
    return retval;
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
    return (dmin <= sqrRadius);
}

GridBox Sphere::GetBoundingBox()
{
    glm::vec3 pos(position - glm::vec3(radius));
    glm::vec3 size(glm::vec3(radius) * 2.f);
    return GridBox(pos, size);
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

                Texture *t=this->GetTexture();
                if(t->isEnabled()==true)
                {
                    unsigned char *texture=t->getTexture();
                    glm::vec3 planePos(contactPoint - this->position);

                    //old version
                    /*int texPosX = glm::clamp((int)(planePos.x / radius * t->getWidth()) + t->getWidth()/2, 0, t->getWidth());
                    int texPosY = glm::clamp((int)(planePos.y / radius * t->getHeight()) + t->getHeight()/2, 0, t->getHeight());
                    float c = texture[(texPosY * t->getHeight() + texPosX)]/255.f;*/

                    ////new version
                    int xx=((int)(planePos.x / radius * t->getWidth()) + t->getWidth()/2)%t->getWidth();
                    int yy=((int)(planePos.y / radius * t->getHeight()) + t->getHeight()/2)%t->getHeight();
                    if(xx<0)
                        xx*=-1;
                    if(yy<0)
                        yy*=-1;
                    float c = texture[(yy * t->getHeight() + xx)]/255.f;

                    material.SetColor(glm::vec4(c, c, c, 1));
                }

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
    return (dmin <= sqrRadius);
}

GridBox Particle::GetBoundingBox()
{
    //using sphere aproximation
    glm::vec3 pos(position - glm::vec3(radius));
    glm::vec3 size(glm::vec3(radius) * 2.f);
    return GridBox(pos, size);
}


int GridBox::Intersect(Ray& ray, float& distance)
{
    //m_RayID = a_Ray.GetID();
    float dist[6];
    glm::vec3 ip[6], d = ray.GetDirection(), o = ray.GetOrigin();
    bool retval = INTERSECTION_RES_MISS;
    for ( int i = 0; i < 6; i++ ) 
        dist[i] = -1.f;
    glm::vec3 v1 = this->position, v2 = this->position + this->size;
    if (d.x) 
    {
        float rc = 1.0f / d.x;
        dist[0] = (v1.x - o.x) * rc;
        dist[3] = (v2.x - o.x) * rc;
    }
    if (d.y) 
    {
        float rc = 1.0f / d.y;
        dist[1] = (v1.y - o.y) * rc;
        dist[4] = (v2.y - o.y) * rc;
    }
    if (d.z) 
    {
        float rc = 1.0f / d.z;
        dist[2] = (v1.z - o.z) * rc;
        dist[5] = (v2.z - o.z) * rc;
    }
    for (int i = 0; i < 6; i++ )
    {
        if (dist[i] > 0)
        {
            ip[i] = o + dist[i] * d;
            if ((ip[i].x > (v1.x - EPSILON)) && (ip[i].x < (v2.x + EPSILON)) && 
                (ip[i].y > (v1.y - EPSILON)) && (ip[i].y < (v2.y + EPSILON)) &&
                (ip[i].z > (v1.z - EPSILON)) && (ip[i].z < (v2.z + EPSILON)))
            {
                if (dist[i] < distance) 
                {
                    distance = dist[i];
                    retval = INTERSECTION_RES_HIT_OUTSIDE;
                }
            }
        }
    }
    return retval;
}
