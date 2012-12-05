#pragma once

#include <glm\glm.hpp>
#include "Ray.h"
#include "Utils.h"
#include "Defines.h"
#include "PerlinNoise.h"

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

class Texture
{
public:
	Texture(void) 
        : width(256), height(256), enabled(false), tex(NULL)
    { }

	Texture(bool enabled) 
        : width(256), height(256), enabled(enabled), tex(NULL)
    { }

    Texture(int width, int height, bool enabled) 
        : width(width), height(height), enabled(enabled), tex(NULL)
    { }

    Texture(int width, int height) 
        : width(width), height(height), enabled(false), tex(NULL)
    { }


	~Texture(void)
    {
        if(this->enabled)
            delete [] tex;
    }

    void setSize(int width, int height)
    { 
        this->width=width;
        this->height=height;
    }

    void setHeight(int height) { this->height = height; }
    void setWidth(int width) { this->width = width;}
    int getWidth(void) { return this->width; }   
    int getHeight(void) { return this->height; }

    bool isEnabled(void) { return this->enabled; }
    void setEnabled(bool enabled) { this->enabled = enabled;}

    void setSmoothNoise(bool smooth) { this->noise.setSmoothNoise(smooth); } //if true consume more CPU then default noise

    void setExpCurve(bool curve) { this->noise.setExpCurve(curve); }
    void setExpCurve(bool curve,int texCover, double texSharpness) { this->noise.setExpCurve(curve, texCover, texSharpness); }
    void setExpCurve(int texCover, double texSharpness) { this->noise.setExpCurve(texCover, texSharpness); }

    void setZoom(int zoom){ this->noise.setZoom(zoom);}
    void setPersistance(double persistance) {this->noise.setPersistance(persistance);}
    void setZoomPersistance(int zoom, double persistance){ this->noise.setZoomPersistance(zoom, persistance);}
    
    unsigned char *getTexture(void)
    {
        if(!this->enabled)
            return NULL;
        else
            return tex;
    }



    void generateTexture(void)
    {
        if(this->enabled)
        {
           tex=noise.generate(width, height);
        }
    }

private:
    int width;
    int height;
    bool enabled;
    PerlinNoise noise;
    unsigned char* tex;
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
    Texture* GetTexture() { return &texture; }
    void SetTexture(Texture tex) 
    {
        this->texture = tex;
    }

    virtual glm::vec4 GetColor(glm::vec3& pos, glm::vec3& origin)
    {
        return material.GetColor();
    }


    virtual glm::vec3 GetNormal(glm::vec3 dir) = 0;
    virtual int Intersect(Ray& ray, float& dist) = 0;
    virtual bool IntersectBox(GridBox& box) = 0;
    virtual GridBox GetBoundingBox() = 0;


protected:
    Material material;
    bool isLight;
    int rayID;
    Texture texture;
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
        return glm::normalize((dir - this->position) * this->revRadius); 
    }

    glm::vec4 GetColor(glm::vec3& pos, glm::vec3& origin);

    int Intersect(Ray& ray, float& dist);
    bool IntersectBox(GridBox& box);
    GridBox GetBoundingBox();


private:

    float getTextureDensity(glm::vec3& pos);
    int intersect(Ray& ray, float& dist);

    glm::vec3 position;
    float radius;
    float sqrRadius;
    float revRadius;
};

class Particle : public Primitive
{
public:
    Particle(glm::vec3 position, glm::vec3 normal, float radius)
        :position(position), radius(radius), normal(normal), sqrRadius(radius * radius)
    {

    }

    ~Particle(void) 
    { 

    };

    float GetRadius() { return radius; }
    void SetRadius(float radius)
    {
        this->radius = radius;
        this->sqrRadius = radius * radius;
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
    
    float sqrRadius;
    
};


