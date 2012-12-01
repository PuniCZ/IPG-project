#pragma once

#include "Primitive.h"
#include <vector>
#include <list>


class ObjectList
{
public:
    ObjectList() 
      : primitive(NULL), next(NULL)
    { }

    ~ObjectList() { delete next; }

    void SetPrimitive(Primitive* primitive) 
    { 
        this->primitive = primitive;
    }
    Primitive* GetPrimitive() { return primitive; }
    void SetNext(ObjectList* next) 
    { 
        this->next = next;
    }
    ObjectList* GetNext() { return next; }
private:
    Primitive* primitive;
    ObjectList* next;
};


class Scene
{
public:
    Scene(void)
        : fogFactor(0.2f)
    { }
    ~Scene(void);

    std::vector<Primitive*>* GetPrimitives() { return &primitives; }
    std::list<Primitive*>* GetLigths() { return &lights; }
    GridBox& GetBoundigBox() { return boundingBox; }
    ObjectList** GetGrid() { return grid; }

    float GetFogFactor() { return fogFactor; }
    void SetFogFactor(float factor)
    {
        fogFactor = factor;
    }

    void Init();
    void BuildGrid();

private:
    std::vector<Primitive*> primitives;

    ObjectList** grid;
    std::list<Primitive*> lights;
    GridBox boundingBox;

    float fogFactor;
};

