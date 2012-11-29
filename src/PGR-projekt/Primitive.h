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


    virtual glm::vec3 GetNormal(glm::vec3 dir) = 0;
    virtual int Intersect(Ray& ray, float& dist) = 0;


protected:
    Material material;
    bool isLight;
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

private:
    glm::vec3 position;
    glm::vec3 normal;
    float radius;
    
    unsigned char* texture;
};


