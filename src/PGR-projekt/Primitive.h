#pragma once

#include <glm\glm.hpp>
#include "Ray.h"

class Material
{
public:
    Material()
        :color(1, 0, 1, 1), diffuse(0.2f)
    { }

    Material(glm::vec4 color, float diffuse)
        :color(color), diffuse(diffuse)
    { }

    Material(glm::vec4 color)
        :color(color), diffuse(0.2f)
    { }

    ~Material() { };

    glm::vec4 GetColor() { return color; }
    float GetDiffuse() { return diffuse; }

private:
    glm::vec4 color;
    float diffuse;


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


    virtual glm::vec3 GetNormal(glm::vec3 pos) = 0;
    virtual bool Intersect(Ray& ray, float& dist) = 0;


protected:
    Material material;
    bool isLight;
};

class Plane : public Primitive
{
public:
    Plane(glm::vec3 normal, float distance)
        :normal(normal), distance(distance)
    { }

    ~Plane(void) { };

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

    bool Intersect(Ray& ray, float& dist);

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

    glm::vec3 GetNormal(glm::vec3 pos) 
    { 
        return (pos - this->position) * this->revRadius; 
    }

    bool Intersect(Ray& ray, float& dist);

private:
    glm::vec3 position;
    float radius;
    float sqrRadius;
    float revRadius;
};


