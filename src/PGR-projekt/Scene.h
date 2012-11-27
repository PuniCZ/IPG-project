#pragma once

#include "Primitive.h"
#include <vector>

class Scene
{
public:
    Scene(void);
    ~Scene(void);

    std::vector<Primitive*>* GetPrimitives() { return &primitives; }

    void Init();

private:
    std::vector<Primitive*> primitives;
};

