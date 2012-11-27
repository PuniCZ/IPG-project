#include "Raytracer.h"



Raytracer::Raytracer(CameraPlane* camera)
{
    this->camera = camera;

    screenX1 = -camera->GetWidth()/200;
    screenX2 = camera->GetWidth()/200;
    screenY1 = camera->GetHeight()/200;
    screenY2 = -camera->GetHeight()/200;

    screenDiffX = (screenX2 - screenX1) / camera->GetWidth();
    screenDiffY = (screenY2 - screenY1) / camera->GetHeight();

    posY = screenY1;
    curLine = 0;
}


Raytracer::~Raytracer(void)
{
}

bool Raytracer::Render(Scene& scene)
{
    glm::vec4 color;

    for (int y = curLine; y < camera->GetHeight(); y++)
    {
        posX = screenX1;
        for (int x = 0; x < camera->GetWidth(); x++)
        {
            glm::vec3 dir(posX, posY, 0);
            dir = glm::normalize(dir - camera->GetOrigin());
            Ray ray(camera->GetOrigin(), dir);
            float distance = INT_MAX;
            color = Raytrace(scene, ray, camera->GetBgColor(), 0, distance);
            color = glm::clamp(color, 0.f, 1.f);
            camera->GetBuffer()->SetPixel(x, y, color);
            posX += screenDiffX;
        }
        posY += screenDiffY;
        curLine++;
        //return false;
    }
    return true;
}

glm::vec4 Raytracer::Raytrace(Scene& scene, Ray& ray, glm::vec4&color, int depth, float& distance)
{
    Primitive* hitObject = NULL;
    glm::vec4 tmpColor = color;
    glm::vec3 pi;

    for (std::vector<Primitive*>::iterator it = scene.GetPrimitives()->begin(); it != scene.GetPrimitives()->end(); it++)
    {
        if ((*it)->Intersect(ray, distance))
        {
            hitObject = *it;
            //tmpColor += ((*it)->GetMaterial()->GetColor()/(distance/5));
        }
    }

    if (hitObject)
    {
        if (hitObject->IsLigth())
        {
            //looking into light
            tmpColor = hitObject->GetMaterial()->GetColor();
        }
        else
        {
            pi = ray.GetOrigin() + ray.GetDirection() * distance;
            //trace lights
            for (std::vector<Primitive*>::iterator it = scene.GetPrimitives()->begin(); it != scene.GetPrimitives()->end(); it++)
            {
                if ((*it)->IsLigth())
                {
                    Sphere* light = (Sphere*)*it;
                    glm::vec3 L(glm::normalize(light->GetPosition() - pi));
                    glm::vec3 N(hitObject->GetNormal(pi));
                    if (hitObject->GetMaterial()->GetDiffuse() > 0.f)
                    {
                        float dot = glm::dot(N, L);
                        if (dot > 0.f)
                        {
                            float diffFactor = dot * hitObject->GetMaterial()->GetDiffuse();
                            tmpColor += diffFactor * hitObject->GetMaterial()->GetColor() * light->GetMaterial()->GetColor();
                        }
                    }
                      

                }
            }
        }
    }

    return tmpColor;
}
