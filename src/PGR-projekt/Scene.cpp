#include "Scene.h"


Scene::Scene(void)
{
}


Scene::~Scene(void)
{
    while(primitives.empty())
    {
        delete primitives.back();
        primitives.pop_back();
    }

}

void Scene::Init()
{
    /*primitives.push_back(new Plane(glm::vec3(0, 0, -1), 25.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(0, 1, 0, 1), 1.f));


    primitives.push_back(new Plane(glm::vec3(0, 1, 0), -5.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(1, 0, 0, 1), 1.f));

    primitives.push_back(new Sphere(glm::vec3(0, 2, 5), 1.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(0, 1, 1, 1), .5f));

    primitives.push_back(new Sphere(glm::vec3(0, -4, -10), 1.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(1, 1, 0, 1), 0.f));
    primitives.back()->SetLigth(true);*/


    primitives.push_back(new Plane(glm::vec3(0, 1, 0), 4.4f));
    primitives.back()->SetMaterial(Material(glm::vec4(.4f, .3f, .3f, 1), 1.f));

    primitives.push_back(new Sphere(glm::vec3(1, -.8f, 3), 2.5f));
    primitives.back()->SetMaterial(Material(glm::vec4(.7f, .7f, .7f, 1)));

    //primitives.push_back(new Sphere(glm::vec3(-5.5f, -.5f, 7), 2.0f));
    //primitives.back()->SetMaterial(Material(glm::vec4(.7f, .7f, 1.f, 1), 0.1f));

    primitives.push_back(new Sphere(glm::vec3(-8, 5, 15), 0.4f));
    primitives.back()->SetMaterial(Material(glm::vec4(.6f, .6f, .6f, 1)));
    primitives.back()->SetLigth(true);

    primitives.push_back(new Sphere(glm::vec3(2, 5, 1), 0.1f));
    primitives.back()->SetMaterial(Material(glm::vec4(.7f, .7f, .9f, 1)));
    primitives.back()->SetLigth(true);


    primitives.push_back(new Sphere(glm::vec3(0, 0, -1), 0.1f));
    primitives.back()->SetMaterial(Material(glm::vec4(.9f, .9f, .9f, 1), 10));
    primitives.back()->SetLigth(true);

    primitives.push_back(new Particle(glm::vec3(-5.5f, 1.5f, 7), glm::vec3(0, 0, 1), 2.0f));
    primitives.back()->SetMaterial(Material(glm::vec4(.2f, 1.f, .2f, 1), .6f));



}
