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
        return false;
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
				    glm::vec4 rcol(0, 0, 0, 0);
                    float dist = INT_MAX;
				    rcol = Raytrace(scene, Ray( pi + R * EPSILON, R ), rcol, depth + 1, dist);
				    tmpColor += refl * rcol * hitObject->GetMaterial()->GetColor();
			    }
		    }

        }
    }

    return tmpColor;
}
