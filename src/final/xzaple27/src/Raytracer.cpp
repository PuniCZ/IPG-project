#include "Raytracer.h"


Raytracer::Raytracer(CameraPlane* camera, Scene& scene)
    : scene(scene)
{
    this->camera = camera;

    screenX1 = -camera->GetWidth()/200;
    screenX2 = camera->GetWidth()/200;
    screenY1 = camera->GetHeight()/200;
    screenY2 = -camera->GetHeight()/200;

    screenDiffX = (screenX2 - screenX1) / camera->GetWidth();
    screenDiffY = (screenY2 - screenY1) / camera->GetHeight();

    Init();

    // precalculate 1 / size of a cell
    cellSizeRev = ((float)GRIDSIZE) / scene.GetBoundigBox().GetSize();
    // precalculate size of a cell
    cellSize = scene.GetBoundigBox().GetSize() * (1.0f / GRIDSIZE);

}


void Raytracer::Init()
{
    posY = screenY1;
    curLine = 0;
    printf("Renderuji scenu...\n");
}


Raytracer::~Raytracer(void)
{
}

bool Raytracer::Render()
{
    glm::vec4 color;
    RaytracerResult lastResult;
    std::vector<RaytracerResult> lastLineResults((int)camera->GetWidth());

    for (int y = curLine; y < camera->GetHeight(); y++)
    {
        posX = screenX1;
        for (int x = 0; x < camera->GetWidth(); x++)
        {
            glm::vec3 dir(posX, posY, 0);
            dir = glm::normalize(dir - camera->GetOrigin());
            Ray ray(camera->GetOrigin(), dir);
            float distance = (float)INT_MAX;
            color = camera->GetBgColor();
            RaytracerResult hitResult = RenderRay(glm::vec3(posX, posY, 0), color);

            //optimized supersampling
            if (camera->GetUseSuperSampling() && (hitResult != lastResult || (y > 0 && lastLineResults[x] != hitResult)))
            {
                lastResult = hitResult;
                lastLineResults[x] = hitResult;
                glm::vec4 tmpColor(0);
                RenderRay(glm::vec3(posX - screenDiffX / 2.0f, posY, 0), tmpColor);
                RenderRay(glm::vec3(posX, posY - screenDiffY / 2.0f, 0), tmpColor);
                RenderRay(glm::vec3(posX - screenDiffX / 2.0f, posY - screenDiffY / 2.0f, 0), tmpColor);
                color = tmpColor / 4;
            }
            
            float factor = 1.f - y / camera->GetHeight();
            glm::vec4 srcC = factor * glm::vec4(.5f, .7f, .9f, 1.f) + (1-factor)*glm::vec4(.23f, .35f, .5f, 1.f);


            applyFarFilter(color, hitResult.distance, srcC);
            applyFog(color, hitResult.distance);
            color = glm::clamp(color, 0.f, 1.f);
            camera->GetBuffer()->SetPixel(x, y, color);
            posX += screenDiffX;
        }
        posY += screenDiffY;
        curLine++;

        //possible output filtering
        /*for (int y = curLine; y < camera->GetHeight(); y++)
        {
            int size = 3;
            for (int x = 0; x < camera->GetWidth(); x++)
            {
                for (int yy = y-size; yy <= y+size; yy++) for (int xx = x-size; xx <= x+size; xx++) 
                {
                    color += glm::vec4(camera->GetBuffer()->GetPixel(glm::clamp(xx, 0, (int)camera->GetWidth()), glm::clamp(yy, 0, (int)camera->GetHeight()))/(size*size*4.f), 1.f);
                }
            }
        }*/


        return false;
    }    
    return true;
}

RaytracerResult Raytracer::Raytrace(Ray& ray, glm::vec4&color, int depth, float& distance, float refractionIndex)
{
    Primitive* hitObject = NULL;
    glm::vec4 tmpColor = color;
    glm::vec3 pi;

    int intersectionResult;

    int reflectedObjects = 0;
    int visibleLights = 0;

    
#ifndef USE_GRID
    for (std::vector<Primitive*>::iterator it = scene.GetPrimitives()->begin(); it != scene.GetPrimitives()->end(); it++)
    {
        if (intersectionResult = (*it)->Intersect(ray, distance))
        {
            hitObject = *it;
        }
    }
#else  
    intersectionResult = FindNearest(ray, distance, hitObject);
#endif
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
            for (std::list<Primitive*>::iterator it = scene.GetLigths()->begin(); it != scene.GetLigths()->end(); it++)
            {
                Sphere* light = (Sphere*)*it;
                glm::vec3 L(light->GetPosition() - pi);
                    
                //point light source 
                float shade = 1.0f;
                float tdist = LENGTH(L);
                L *= (1.0f / tdist);

                Ray r = Ray(pi + L * EPSILON, L);
                Primitive* pr = NULL;
               
#ifndef USE_GRID
                for (std::vector<Primitive*>::iterator it2 = scene.GetPrimitives()->begin(); it2 != scene.GetPrimitives()->end(); it2++)
                {
                    if (((*it2) != light) && ((*it2)->Intersect(r, tdist)))
                    {
                        shade = 0;
                        break;
                    }
                }
#else
                FindNearest(r, tdist, pr);
                if (pr != light) 
                    shade = 0;
#endif
                if (shade > 0)
                {
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
                            tmpColor += diff * light->GetMaterial()->GetColor() * hitObject->GetColor(pi, ray.GetOrigin());
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
                            tmpColor += spec * light->GetMaterial()->GetColor() * hitObject->GetColor(pi, ray.GetOrigin()); //TODO: Added
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
                    float dist = (float)INT_MAX;
                    Ray r(pi + R * EPSILON, R);
                    Raytrace(r, rcol, depth + 1, dist, refractionIndex);
                    tmpColor += refl * rcol * hitObject->GetColor(pi, ray.GetOrigin());
                }
            }

            // calculate refraction
            float refr = hitObject->GetMaterial()->GetRefraction();
            if(!(depth < TRACEDEPTH))
            { //max depth reached
                tmpColor=glm::vec4(0.619f,0.83f,1.0f,1);
            }

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
                    float dist = (float)INT_MAX;
                    glm::vec4 transparency;
                    Ray r(pi + T * EPSILON, T);
                    RaytracerResult rtRes =  Raytrace(r, rcol, depth + 1, dist, rindex);

                    // apply Beer's law
                    glm::vec4 absorbance = hitObject->GetColor(pi, ray.GetOrigin()) * 0.25f;// * -dist;
                    transparency = glm::vec4(expf( absorbance.r ), expf( absorbance.g ), expf( absorbance.b ), 1);  
                    
                    // apply far filter and color mixing based on actual depht 
                    float factor = (float)depth / (TRACEDEPTH/2);

                    if (scene.GetFogFactor() > 0.f)
                        factor = 0.f;

                    glm::vec4 srcC = factor * glm::vec4(.7f, .7f, .7f, 1.f) + (1-factor)*glm::vec4(.23f, .35f, .5f, 1.f);
                    
                    factor = 1.f - this->curLine / camera->GetHeight();

                    if (scene.GetFogFactor() > 0.f)
                        factor = 0.f;
                    glm::vec4 srcCC = factor * glm::vec4(.5f, .7f, .9f, 1.f) + (1-factor)*srcC;

                    applyFarFilter(rcol, dist, srcCC);
                    
                    tmpColor += rcol * transparency;
                    reflectedObjects++;
                }
            }

        }
    }


    color = tmpColor;
    return RaytracerResult(hitObject, reflectedObjects, visibleLights, distance);
}


int Raytracer::FindNearest(Ray& ray, float& dist, Primitive*& primitive)
{
    int retval = INTERSECTION_RES_MISS;
    glm::vec3 raydir, curpos;
    GridBox e = scene.GetBoundigBox();
    curpos = ray.GetOrigin();
    raydir = ray.GetDirection();
    // setup 3DDDA
    glm::vec3 cb, tmax, tdelta, cell;
    cell = (curpos - e.GetPos()) * cellSizeRev;
    int stepX, outX, X = (int)cell.x;
    int stepY, outY, Y = (int)cell.y;
    int stepZ, outZ, Z = (int)cell.z;

    if ((X < 0) || (X >= GRIDSIZE) || (Y < 0) || (Y >= GRIDSIZE) || (Z < 0) || (Z >= GRIDSIZE)) 
        return INTERSECTION_RES_MISS;

    if (raydir.x > 0)
    {
        stepX = 1, outX = GRIDSIZE;
        cb.x = e.GetPos().x + (X + 1) * cellSize.x;
    }
    else 
    {
        stepX = -1, outX = -1;
        cb.x = e.GetPos().x + X * cellSize.x;
    }
    if (raydir.y > 0.0f)
    {
        stepY = 1, outY = GRIDSIZE;
        cb.y = e.GetPos().y + (Y + 1) * cellSize.y; 
    }
    else 
    {
        stepY = -1, outY = -1;
        cb.y = e.GetPos().y + Y * cellSize.y;
    }
    if (raydir.z > 0.0f)
    {
        stepZ = 1, outZ = GRIDSIZE;
        cb.z = e.GetPos().z + (Z + 1) * cellSize.z;
    }
    else 
    {
        stepZ = -1, outZ = -1;
        cb.z = e.GetPos().z + Z * cellSize.z;
    }
    float rxr, ryr, rzr;
    if (raydir.x != 0)
    {
        rxr = 1.0f / raydir.x;
        tmax.x = (cb.x - curpos.x) * rxr; 
        tdelta.x = cellSize.x * stepX * rxr;
    }
    else tmax.x = 1000000;
    if (raydir.y != 0)
    {
        ryr = 1.0f / raydir.y;
        tmax.y = (cb.y - curpos.y) * ryr; 
        tdelta.y = cellSize.y * stepY * ryr;
    }
    else tmax.y = 1000000;
    if (raydir.z != 0)
    {
        rzr = 1.0f / raydir.z;
        tmax.z = (cb.z - curpos.z) * rzr; 
        tdelta.z = cellSize.z * stepZ * rzr;
    }
    else tmax.z = 1000000;
    // start stepping
    ObjectList* list = NULL;
    ObjectList** grid = scene.GetGrid();
    primitive = NULL;
    // trace primary ray
    while (1)
    {
        list = grid[X + Y * GRIDSIZE + Z * GRIDSIZE * GRIDSIZE];
        while(list)
        {
            Primitive* pr = list->GetPrimitive();
            int result;
            if (result = pr->Intersect(ray, dist)) 
            {
                retval = result;
                primitive = pr;
                goto testloop;
                
            }
                list = list->GetNext();
        }
        
        if (tmax.x < tmax.y)
        {
            if (tmax.x < tmax.z)
            {
                X = X + stepX;
                if (X == outX) 
                    return INTERSECTION_RES_MISS;
                tmax.x += tdelta.x;
            }
            else
            {
                Z = Z + stepZ;
                if (Z == outZ) 
                    return INTERSECTION_RES_MISS;
                tmax.z += tdelta.z;
            }
        }
        else
        {
            if (tmax.y < tmax.z)
            {
                Y = Y + stepY;
                if (Y == outY) 
                    return INTERSECTION_RES_MISS;
                tmax.y += tdelta.y;
            }
            else
            {
                Z = Z + stepZ;
                if (Z == outZ) 
                    return INTERSECTION_RES_MISS;
                tmax.z += tdelta.z;
            }
        }
    }
testloop:
    while (1)
    {
        list = grid[X + Y * GRIDSIZE + Z * GRIDSIZE * GRIDSIZE];
        while (list)
        {
            Primitive* pr = list->GetPrimitive();
            int result;
            if (result = pr->Intersect(ray, dist)) 
            {
                primitive = pr;
                retval = result;
            }
            list = list->GetNext();
        }
        if (tmax.x < tmax.y)
        {
            if (tmax.x < tmax.z)
            {
                if (dist < tmax.x) break;
                X = X + stepX;
                if (X == outX) break;
                tmax.x += tdelta.x;
            }
            else
            {
                if (dist < tmax.z) break;
                Z = Z + stepZ;
                if (Z == outZ) break;
                tmax.z += tdelta.z;
            }
        }
        else
        {
            if (tmax.y < tmax.z)
            {
                if (dist < tmax.y) break;
                Y = Y + stepY;
                if (Y == outY) break;
                tmax.y += tdelta.y;
            }
            else
            {
                if (dist < tmax.z) break;
                Z = Z + stepZ;
                if (Z == outZ) break;
                tmax.z += tdelta.z;
            }
        }
    }
    return retval;
}

RaytracerResult Raytracer::RenderRay(glm::vec3 screenPos, glm::vec4& color)
{
    GridBox e = scene.GetBoundigBox();
    
    glm::vec3 dir(glm::normalize(screenPos - camera->GetOrigin()));
    Ray r(camera->GetOrigin(), dir);
    // advance ray to scene bounding box boundary
    if (!e.Contains(camera->GetOrigin()))
    {
        float bdist = (float)INT_MAX;
        if (e.Intersect(r, bdist))
            r.SetOrigin(camera->GetOrigin() + (bdist + EPSILON) * dir);
    }
    float dist = (float)INT_MAX;
    return Raytrace(r, color, 1, dist, 1.0f);
}


void Raytracer::applyFog(glm::vec4& color, float distance)
{
    if (scene.GetFogFactor() <= 0.f)
        return;

    glm::vec4 fogColor(.6f, .6f, .6f, 1.f);
    fogColor += RAND(0.02f) - 0.01f;
    float f = expf(-distance * scene.GetFogFactor());
    color = f * color + (1 - f) * fogColor;

}

void Raytracer::applyFarFilter(glm::vec4& color, float distance)
{
    glm::vec4 farC(.23f, .35f, .5f, 1.f);
    applyFarFilter(color, distance, farC);

}

void Raytracer::applyFarFilter(glm::vec4& color, float distance, glm::vec4& srcColor)
{
    if (distance - scene.GetFarDistance() < 0.f)
        return;

    float f = expf(-(distance - scene.GetFarDistance()) * 0.01f);
    color = f * color + (1 - f) * srcColor;

}
