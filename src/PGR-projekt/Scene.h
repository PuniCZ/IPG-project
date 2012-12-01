#pragma once

#include "Primitive.h"
#include <vector>
#include <list>



class Scene
{
public:
    Scene(void);
    ~Scene(void);

    std::vector<Primitive*>* GetPrimitives() { return &primitives; }

    void Init();
    void BuildGrid();

private:
    std::vector<Primitive*> primitives;

    std::list<Primitive*>** grid;
    std::list<Primitive*> lights;
    GridBox boundingBox;
};

