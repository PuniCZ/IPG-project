#pragma once

#include <glm\glm.hpp>
#include "Ray.h"
#include "Utils.h"
#include "Defines.h"

class Material
{
public:
    Material()
        :color(1, 0, 1, 1), diffuse(0.2f), reflection(0.f), refractionIndex(1.5f), refraction(0.f)
    { }

    Material(glm::vec4 color, float diffuse)
        :color(color), diffuse(diffuse), reflection(0.f), refractionIndex(1.5f), refraction(0.f)
    { }

    Material(glm::vec4 color, float diffuse, float reflection)
        :color(color), diffuse(diffuse), reflection(reflection), refractionIndex(1.5f), refraction(0.f)
    { }

    Material(glm::vec4 color, float diffuse, float reflection, float refraction,  float reafractionIndex)
        :color(color), diffuse(diffuse), reflection(reflection), refraction(refraction), refractionIndex(reafractionIndex)
    { }

    Material(glm::vec4 color)
        :color(color), diffuse(0.2f), reflection(0.f), refraction(0.f), refractionIndex(1.5f)
    { }

    ~Material() { };

    glm::vec4 GetColor() { return color; }
    void SetColor(glm::vec4 color) 
    { 
        this->color = color; 
    }

    float GetDiffuse() { return diffuse; }
    void SetDiffuse(float diffuse) 
    { 
        this->diffuse = diffuse; 
    }

    float GetSpecular() { return 1.0f - diffuse; }

    float GetReflection() { return reflection; }
    void SetReflection(float reflection) 
    { 
        this->reflection = reflection; 
    }

    float GetRefraction() { return refraction; }
    void SetRefraction(float refraction) 
    { 
        this->refraction = refraction; 
    }

    float GetRefractionIndex() { return refractionIndex; }
    void SetRefractionIndex(float refractionIndex) 
    { 
        this->refractionIndex = refractionIndex; 
    }

private:
    glm::vec4 color;
    float diffuse;
    float reflection;
    float refraction;
    float refractionIndex;


};


class GridBox
{
public:
    GridBox() 
        : size(0.f), position(0.f)
    { }

    GridBox(glm::vec3& position, glm::vec3& size) 
        : size(size), position(position)
    { }

    glm::vec3& GetPos() { return position; }

    glm::vec3& GetSize() { return size; }

    bool Intersect(GridBox& box)
    {
        glm::vec3 v1 = box.GetPos(), v2 = box.GetPos() + box.GetSize();
        glm::vec3 v3 = position, v4 = position + size;
        return ((v4.x > v1.x) && (v3.x < v2.x) &&  // x overlap
                (v4.y > v1.y) && (v3.y < v2.y) &&  // y overlap
                (v4.z > v1.z) && (v3.z < v2.z));   // z overlap
    }

    bool Contains(glm::vec3 pos)
    {
        glm::vec3 v1 = position, v2 = position + size;
        return ((pos.x > (v1.x - EPSILON)) && (pos.x < (v2.x + EPSILON)) &&
                (pos.y > (v1.y - EPSILON)) && (pos.y < (v2.y + EPSILON)) &&
                (pos.z > (v1.z - EPSILON)) && (pos.z < (v2.z + EPSILON)));
    }

    int Intersect(Ray& ray, float& dist);

private:
    glm::vec3 position;
    glm::vec3 size;
};


class Primitive
{
public:
    Primitive(void);
    ~Primitive(void);

    Material* GetMaterial() { return &material; }
    void SetMaterial(Material mat) 
    {
        this->material = mat;
    }

    bool IsLigth() { return isLight; }
    void SetLigth(bool is) 
    { 
        isLight = is; 
    }

    int GetRayId() { return rayID; }


    virtual glm::vec3 GetNormal(glm::vec3 dir) = 0;
    virtual int Intersect(Ray& ray, float& dist) = 0;
    virtual bool IntersectBox(GridBox& box) = 0;
    virtual GridBox GetBoundingBox() = 0;


protected:
    Material material;
    bool isLight;
    int rayID;
};

class Plane : public Primitive
{
public:
    Plane(glm::vec3 normal, float distance)
        :normal(normal), distance(distance)
    { 
        
    }

    ~Plane(void) 
    {
        
    };

    float GetDistance() { return distance; }
    void SetDistance(float dist)
    {
        distance = dist;
    }

    glm::vec3 GetNormal(glm::vec3 pos) { return normal; }
    void SetNormal(glm::vec3 norm)
    {
        normal = norm;
    }

    int Intersect(Ray& ray, float& dist);
    bool IntersectBox(GridBox& box);
    GridBox GetBoundingBox();

private:
    glm::vec3 normal;
    float distance;
};


class Sphere : public Primitive
{
public:
    Sphere(glm::vec3 position, float radius)
        :position(position), radius(radius), sqrRadius(radius * radius), revRadius(1.f / radius)
    { }

    ~Sphere(void) { };

    float GetRadius() { return radius; }
    void SetRadius(float radius)
    {
        this->radius = radius;
        this->sqrRadius = radius * radius;
        this->revRadius = 1.f / radius;
    }

    glm::vec3 GetPosition() { return position; }
    void SetPosition(glm::vec3 position)
    {
        this->position = position;
    }

    glm::vec3 GetNormal(glm::vec3 dir) 
    { 
        return (dir - this->position) * this->revRadius; 
    }

    int Intersect(Ray& ray, float& dist);
    bool IntersectBox(GridBox& box);
    GridBox GetBoundingBox();

private:
    glm::vec3 position;
    float radius;
    float sqrRadius;
    float revRadius;
};

class Particle : public Primitive
{
public:
    Particle(glm::vec3 position, glm::vec3 normal, float radius)
        :position(position), radius(radius), normal(normal)
    {
        texture = Utils::CreateGaussianMap(256);
    }

    ~Particle(void) 
    { 
        delete[] texture;
    };

    float GetRadius() { return radius; }
    void SetRadius(float radius)
    {
        this->radius = radius;
    }

    glm::vec3 GetPosition() { return position; }
    void SetPosition(glm::vec3 position)
    {
        this->position = position;
    }

    glm::vec3 GetNormal(glm::vec3 dir) 
    { 
        return normal; 
    }

    int Intersect(Ray& ray, float& dist);
    bool IntersectBox(GridBox& box);
    GridBox GetBoundingBox();


private:
    glm::vec3 position;
    glm::vec3 normal;
    float radius;
    
    unsigned char* texture;
};


