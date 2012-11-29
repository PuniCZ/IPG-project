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
    RaytracerResult lastResult;
    std::vector<RaytracerResult> lastLineResults(camera->GetWidth());

    for (int y = curLine; y < camera->GetHeight(); y++)
    {
        posX = screenX1;
        for (int x = 0; x < camera->GetWidth(); x++)
        {
            glm::vec3 dir(posX, posY, 0);
            dir = glm::normalize(dir - camera->GetOrigin());
            Ray ray(camera->GetOrigin(), dir);
            float distance = INT_MAX;
            color = camera->GetBgColor();
            RaytracerResult hitResult = Raytrace(scene, ray, color, 0, distance, 1.0f);

            //optimized supersampling
            if (camera->GetUseSuperSampling() && (hitResult != lastResult || (y > 0 && lastLineResults[x] != hitResult)))
            {
                lastResult = hitResult;
                lastLineResults[x] = hitResult;
                glm::vec4 tmpColor(0);
                for ( int tx = -1; tx < 2; tx++ ) for ( int ty = -1; ty < 2; ty++ )
                {
                    glm::vec3 dir(posX + screenDiffX * tx / 2.0f, posY + screenDiffY * ty / 2.0f, 0);
                    dir = glm::normalize(dir - camera->GetOrigin());
                    Ray ray(camera->GetOrigin(), dir);
                    float distance = INT_MAX;
                    Raytrace(scene, ray, tmpColor, 0, distance, 1.0f);
                }
                color = tmpColor / 9;
            }

            color = glm::clamp(color, 0.f, 1.f);
            camera->GetBuffer()->SetPixel(x, y, color);
            posX += screenDiffX;
        }
        posY += screenDiffY;
        curLine++;
        return false;
    }
    return true;
}

RaytracerResult Raytracer::Raytrace(Scene& scene, Ray& ray, glm::vec4&color, int depth, float& distance, float refractionIndex)
{
    Primitive* hitObject = NULL;
    glm::vec4 tmpColor = color;
    glm::vec3 pi;

    int intersectionResult;

    int reflectedObjects = 0;
    int visibleLights = 0;

    for (std::vector<Primitive*>::iterator it = scene.GetPrimitives()->begin(); it != scene.GetPrimitives()->end(); it++)
    {
        if (intersectionResult = (*it)->Intersect(ray, distance))
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
            tmpColor += hitObject->GetMaterial()->GetColor();
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
                    glm::vec3 L(light->GetPosition() - pi);
                    
                    //point light source                    
                    float shade = 1.0f;
                    float tdist = LENGTH(L);
                    L *= (1.0f / tdist);

                    Ray r = Ray(pi + L * EPSILON, L);
                    for (std::vector<Primitive*>::iterator it2 = scene.GetPrimitives()->begin(); it2 != scene.GetPrimitives()->end(); it2++)
                    {
                        if (((*it2) != light) && ((*it2)->Intersect(r, tdist)))
                        {
                            shade = 0;
                            break;
                        }
                    }

                    // calculate diffuse shading
                    L = glm::normalize((light->GetPosition() - pi));
                    glm::vec3 N = hitObject->GetNormal(pi);
                    if ((*it)->GetMaterial()->GetDiffuse() > 0)
                    {
                        float dot = glm::dot(L, N);
                        if (dot > 0)
                        {
                            float diff = dot * hitObject->GetMaterial()->GetDiffuse() * shade;
                            // add diffuse component to ray color
                            tmpColor += diff * light->GetMaterial()->GetColor() * hitObject->GetMaterial()->GetColor();
                            visibleLights++;
                        }
                    }

                    // determine specular component
                    if (hitObject->GetMaterial()->GetSpecular() > 0)
                    {
                        // point light source: sample once for specular highlight
                        glm::vec3 V(ray.GetDirection());
                        glm::vec3 R(L - 2.0f * glm::dot(L, N) * N);
                        float dot = glm::dot(V, R);
                        if (dot > 0)
                        {
                            float spec = powf( dot, 20 ) * hitObject->GetMaterial()->GetSpecular() * shade;
                            // add specular component to ray color
                            tmpColor += spec * light->GetMaterial()->GetColor();
                        }
                    }
                }
            } //end lights

            // calculate reflection
            float refl = hitObject->GetMaterial()->GetReflection();
            if (refl > 0.0f)
            {
                glm::vec3 N(hitObject->GetNormal(pi));
                glm::vec3 R(ray.GetDirection() - 2.0f * glm::dot(ray.GetDirection(), N) * N);
                if (depth < TRACEDEPTH) 
                {
                    glm::vec4 rcol(0);
                    float dist = INT_MAX;
                    Raytrace(scene, Ray( pi + R * EPSILON, R ), rcol, depth + 1, dist, refractionIndex);
                    tmpColor += refl * rcol * hitObject->GetMaterial()->GetColor();
                }
            }

            // calculate refraction
            float refr = hitObject->GetMaterial()->GetRefraction();
            if ((hitObject->GetMaterial()->GetRefraction() > 0) && (depth < TRACEDEPTH))
            {
                float rindex = hitObject->GetMaterial()->GetRefractionIndex();
                float n = refractionIndex / rindex;
                glm::vec3 N = hitObject->GetNormal( pi ) * (float)intersectionResult;
                float cosI = -glm::dot(N, ray.GetDirection());
                float cosT2 = 1.0f - n * n * (1.0f - cosI * cosI);
                if (cosT2 > 0.0f)
                {
                    glm::vec3 T((n * ray.GetDirection()) + (n * cosI - sqrtf( cosT2 )) * N);
                    glm::vec4 rcol(0);
                    float dist;
                    Raytrace(scene, Ray( pi + T * EPSILON, T ), rcol, depth + 1, rindex, dist);
                    // apply Beer's law
                    glm::vec4 absorbance = hitObject->GetMaterial()->GetColor() * 0.15f * -dist;
                    glm::vec4 transparency(expf( absorbance.r ), expf( absorbance.g ), expf( absorbance.b ), 1);
                    tmpColor += rcol * transparency;

                    reflectedObjects++; //TODO: needs pass from recursive call
                }
            }

        }
    }


    color = tmpColor;
    return RaytracerResult(hitObject, reflectedObjects, visibleLights);
}
