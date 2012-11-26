#include "Cloud.h"
#include <time.h>
#include <algorithm>
#include <glm/gtx/vector_angle.hpp>

Cloud::Cloud(glm::vec3 position, glm::vec3 size)
    :centerPosition(position), radius(std::max(std::max(size.x, size.y), size.z))
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < PARTICLES_IN_CLOUD; i++)
    {
        particles.push_back(CloudParticle(position, size));
    }
}


Cloud::~Cloud(void)
{
}


void Cloud::CopyParticlesToBuffer(GLuint VBO, int offset)
{    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);   
    //fill buffer
    float *buffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    int bufferOffset = offset;    
    for (std::vector<CloudParticle>::iterator it = particles.begin(); it != particles.end(); it++)
    {
        for (int i = 0; i < 4; i++)
        {
            memcpy(buffer + bufferOffset + 0, glm::value_ptr(it->RenderablePoints[i].color), sizeof(float)*4);
            memcpy(buffer + bufferOffset + 4, glm::value_ptr(it->RenderablePoints[i].position), sizeof(float)*3);
            memcpy(buffer + bufferOffset + 7, glm::value_ptr(it->RenderablePoints[i].textCoord), sizeof(float)*2);
            bufferOffset += 9; //Important: if changed, change also PARTICLE_STRUCT_SIZE
        }        
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}


//------------------------------------------------------------------------------
// Function     	  : SkyCloud::_SortParticles
// Description	    : 
//------------------------------------------------------------------------------
/**
 * @fn SkyCloud::_SortParticles(const Vec3f& vecViewDir, const Vec3f& sortPoint, SortDirection dir)
 * @brief Sorts the cloud particles in the direction specified by @a dir.
 * 
 * @vecSortPoint is assumed to already be transformed into the basis space of the cloud.
 */ 
void Cloud::SortParticles(const glm::vec3& viewDir, const glm::vec3& vecSortPoint, bool dirToward)
{
    glm::vec3 partPos;
    for (std::vector<CloudParticle>::iterator it = particles.begin(); it != particles.end(); it++)
    {
	    partPos = it->GetPosition();
	    partPos -= vecSortPoint;
        it->SetSquareSortDistance(glm::dot(partPos, viewDir));//partPos.LengthSqr());
    }

    if (dirToward)
    {
        std::sort(particles.begin(), particles.end(), towardComparator);
    }
    else //away
    {
        std::sort(particles.begin(), particles.end(), awayComparator);
    }
}

bool Cloud::Update(glm::vec3 newViewPos)
{
    //TODO
    return false;

    glm::vec3 newNormalized = glm::normalize(newViewPos);

    if (needsToBeUpdated(newNormalized))
    {
        for (std::vector<CloudParticle>::iterator it = particles.begin(); it != particles.end(); it++)
        {
            for (int i = 0; i < 4; i++)
            {
                //TODO
                //it->RenderablePoints[i].position = glm::rotate(it->RenderablePoints[i].position, acos(glm::dot(newNormalized, lastViewPos)), glm::vec3(0,1,0));
                //it->Update();                
            }
        }
        lastViewPos = newNormalized;
        return true;
    }
    return false;
}


//------------------------------------------------------------------------------
// Function     	  : SkyCloud::Illuminate
// Description	    : 
//------------------------------------------------------------------------------
/**
 * @fn SkyCloud::Illuminate(SkyLight *pLight, SkyRenderableInstance* pInstance, bool bReset)
 * @brief Compute the illumination of the cloud by the lightsource @a pLight
 * 
 * This method uses graphics hardware to compute multiple forward scattering at each cloud
 * in the cloud of light from the directional light source @a pLight.  The algorithm works
 * by successively subtracting "light" from an initially white (fully lit) frame buffer by
 * using hardware blending and read back.  The method stores the illumination from each light
 * source passed to it separately at each particle, unless @a bReset is true, in which case
 * the lists of illumination in the particles are reset before the lighting is computed.
 * 
 */ 
void Cloud::Illuminate(Light& light, bool isFirstLight)
{
    //TODO: need to keep old mvp
  
    
    
    /*glm::mat4 mv = glm::scale(
            glm::rotate(
                glm::rotate(
                    glm::translate(
                        glm::mat4(1.0f),
                        glm::vec3(0, 0, pz)
                        ),
                    ry, glm::vec3(1, 0, 0)
                    ),
                rx, glm::vec3(0, 1, 0)
                ), glm::vec3(min(pow(1.5f, wheel), 50.0f))
            );*/
    

  //glViewport(0, 0, s_iShadeResolution, s_iShadeResolution);

    glm::vec3 vecDir(light.GetDirection());

  // if this is the first pass through the lights, reset will be true, and the cached light 
  // directions should be updated.  Light directions are cached in cloud space to accelerate 
  // computation of the phase function, which depends on light direction and view direction.
    if (isFirstLight)
        lightDirections.clear();
    lightDirections.push_back(vecDir); // cache the (unit-length) light direction

  // compute the light/sort position for particles from the light direction.
  // don't just use the camera position -- if it is too far away from the cloud, then
  // precision limitations may cause the STL sort to hang.  Instead, put the sort position
  // just outside the bounding sphere of the cloud in the direction of the camera.
    glm::vec3 vecLightPos(vecDir);
    //vecLightPos *= (1.1*_boundingBox.GetRadius());
    vecLightPos *= (1.1 * radius);
    vecLightPos += centerPosition;

  // Set up a camera to look at the cloud from the light position.  Since the sun is an infinite
  // light source, this camera will use an orthographic projection tightly fit to the bounding
  // sphere of the cloud.  
  
    //Camera cam;
    
    glm::mat4 mv;
  
  // Avoid degenerate camera bases.
    glm::vec3 vecUp(0, 1, 0);
    if (fabs(glm::dot(vecDir, vecUp)) - 1 < 1e-6) // check that the view and up directions are not parallel.
        vecUp = glm::vec3(1, 0, 0);
    mv = glm::lookAt(vecLightPos, centerPosition, vecUp);
    //cam.LookAt(vecLightPos, _boundingBox.GetCenter(), vecUp);

    glm::vec3 viewDirection = vecLightPos - centerPosition;
    viewDirection = glm::normalize(viewDirection);

  // sort the particles away from the light source.
  //_SortParticles(cam.ViewDir(), vecLightPos, SKY_CLOUD_SORT_AWAY);
    SortParticles(viewDirection, vecLightPos, false);

  // projected dist to cntr along viewdir
  //float DistToCntr = (_boundingBox.GetCenter() - vecLightPos) * cam.ViewDir();
    float DistToCntr = glm::dot((centerPosition - vecLightPos), viewDirection);
  
  // calc tight-fitting near and far distances for the orthographic frustum
  //float rNearDist = DistToCntr - _boundingBox.GetRadius();
  //float rFarDist  = DistToCntr + _boundingBox.GetRadius();
    float rNearDist = DistToCntr - radius;
    float rFarDist  = DistToCntr + radius;

  // set the modelview matrix from this camera.
  /*glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  float M[16];
  cam.GetModelviewMatrix(M);
  glLoadMatrixf(M);*/
  
  // switch to parallel projection
  //glm::mat4 projection = glm::perspective(45.0f, (float)SHADE_RESOLUTION/(float)SHADE_RESOLUTION, 1.0f, 1000.0f); //TODO
  /*glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-_boundingBox.GetRadius(), _boundingBox.GetRadius(),
          -_boundingBox.GetRadius(), _boundingBox.GetRadius(),
          rNearDist, rFarDist);*/
    glm::mat4 projection = glm::ortho(-radius, radius, -radius, radius, rNearDist, rFarDist);

    glm::mat4 mvp = projection * mv;
    
    glUniformMatrix4fv(Utils::glMvpUniform, 1, GL_FALSE, glm::value_ptr(mvp)); 
  
  // set the material state / properties that clouds use for shading:
  // Enables blending, with blend func (ONE, ONE_MINUS_SRC_ALPHA).
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Enables alpha test to discard completely transparent fragments.
    glAlphaFunc(GL_GREATER, 0);
    glEnable(GL_ALPHA_TEST);
  // Disables depth test.
    glDisable(GL_DEPTH_TEST);
  // Enables texturing, with modulation, and the texture set to the shared splat texture.
  //s_pShadeMaterial->Activate();
    //TODO
    
  
  // these are used for projecting the particle position to determine where to read pixels.
    double MM[16], PM[16];
    int VP[4] = { 0, 0, SHADE_RESOLUTION, SHADE_RESOLUTION };
  //glGetDoublev(GL_MODELVIEW_MATRIX, MM);
  //glGetDoublev(GL_PROJECTION_MATRIX, PM);
    for (int i = 0; i<16; i++)
    {
        MM[i] = mv[i/4][i%4];
        PM[i] = projection[i/4][i%4];
    }
    
  // initialize back buffer to all white -- modulation darkens areas where cloud particles
  // absorb light, and lightens it where they scatter light in the forward direction.
    glClearColor(.1, .1, .1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float rPixelsPerLength = SHADE_RESOLUTION / (2 * radius);

    //prepare drawing
    glEnableVertexAttribArray(Utils::glPositionAttrib);
    glEnableVertexAttribArray(Utils::glColorAttrib);
    glEnableVertexAttribArray(Utils::glTexCoordsAtrib);

    //texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Utils::glTexture0);
    glUniform1i(Utils::glTextureUniform, 0);

    glVertexAttribPointer(Utils::glColorAttrib, 4, GL_FLOAT, GL_FALSE, PARTICLE_STRUCT_SIZE, (void*)0);
    glVertexAttribPointer(Utils::glPositionAttrib, 3, GL_FLOAT, GL_FALSE, PARTICLE_STRUCT_SIZE, (void*)16);
    glVertexAttribPointer(Utils::glTexCoordsAtrib, 2, GL_FLOAT, GL_FALSE, PARTICLE_STRUCT_SIZE, (void*)28);


  // the solid angle over which we will sample forward-scattered light.
    float rSolidAngle = 0.09;
    int i = 0;
    int iNumFailed = 0;
    for (std::vector<CloudParticle>::iterator it = particles.begin(); it != particles.end(); it++, i++)
    {
        glm::vec3 vecParticlePos = it->GetPosition();

        glm::vec3 vecOffset(vecLightPos);
        vecOffset -= vecParticlePos;
   
        // compute the pixel area to read back in order to integrate the illumination of the particle
        // over a constant solid angle.
        //float rDistance  = fabs(cam.ViewDir() * vecOffset) - rNearDist;
        float rDistance  = fabs(glm::dot(viewDirection, vecOffset)) - rNearDist;

        float rArea      = rSolidAngle * rDistance * rDistance;
        int   iPixelDim  = sqrt(rArea) * rPixelsPerLength;
        int   iNumPixels = iPixelDim * iPixelDim;
        if (iNumPixels < 1) 
        {
          iNumPixels = 1;
          iPixelDim = 1;
        }

        // the scale factor to convert the read back pixel colors to an average illumination of the area.
        float rColorScaleFactor = rSolidAngle / (iNumPixels * 255.0f);

        unsigned char *c = new unsigned char[4 * iNumPixels];
    
        
        
        double vecWinPosX, vecWinPosY, vecWinPosZ;

        // find the position in the buffer to which the particle position projects.
        /*if (!gluProject(vecParticlePos.x, vecParticlePos.y, vecParticlePos.z, 
                        MM, PM, VP, 
                        &(vecWinPos.x), &(vecWinPos.y), &(vecWinPos.z)))
        {
          FAIL_RETURN_MSG(SKYRESULT_FAIL, 
                          "Error: SkyCloud::Illuminate(): failed to project particle position.");
        }*/
        if (!gluProject(vecParticlePos.x, vecParticlePos.y, vecParticlePos.z, 
                        MM, PM, VP, 
                        &vecWinPosX, &vecWinPosY, &vecWinPosZ))
        {
          /*FAIL_RETURN_MSG(SKYRESULT_FAIL, 
                          "Error: SkyCloud::Illuminate(): failed to project particle position.");*/
            return; //TODO: Chyba
        }

        glm::vec3 vecWinPos(vecWinPosX, vecWinPosY, vecWinPosZ);
   
        // offset the projected window position by half the size of the readback region.
        vecWinPos.x -= 0.5 * iPixelDim;
        if (vecWinPos.x < 0) vecWinPos.x = 0;
        vecWinPos.y -= 0.5 * iPixelDim;
        if (vecWinPos.y < 0) vecWinPos.y = 0;
    
        // read back illumination of this particle from the buffer.
        glReadBuffer(GL_BACK);
        glReadPixels(vecWinPos.x, vecWinPos.y, iPixelDim, iPixelDim, GL_RGBA, GL_UNSIGNED_BYTE, c);
    
        // scattering coefficient vector.
        //Vec4f vecScatter(s_rScatterFactor, s_rScatterFactor, s_rScatterFactor, 1);
        glm::vec4 vecScatter(SCATTER_FACTOR, SCATTER_FACTOR, SCATTER_FACTOR, 1);

        // add up the read back pixels (only need one component -- its grayscale)
        int iSum = 0;
        for (int k = 0; k < 4 * iNumPixels; k+=4)
          iSum += c[k];
        delete [] c;
    
        // compute the amount of light scattered to this particle by particles closer to the light.
        // this is the illumination over the solid angle that we measured (using glReadPixels) times
        // the scattering coefficient (vecScatter);
        /*Vec4f vecScatteredAmount(iSum * rColorScaleFactor, 
                             iSum * rColorScaleFactor, 
                             iSum * rColorScaleFactor, 
                             1 - s_rTransparency);*/
        glm::vec4 vecScatteredAmount(iSum * rColorScaleFactor, 
                             iSum * rColorScaleFactor, 
                             iSum * rColorScaleFactor, 
                             1 - TRANSPARENCY);
        vecScatteredAmount *= vecScatter;

        // the color of th particle (iter) contributed by this light source (pLight) is the 
        // scattered light from the part of the cloud closer to the light, times the diffuse color
        // of the light source.  The alpha is 1 - the uniform transparency of all particles (modulated
        // by the splat texture).
        glm::vec4 vecColor = vecScatteredAmount;
        vecColor      *= light.GetDiffuse();  
        vecColor.w     = 1 - TRANSPARENCY;
    
        // add this color to the list of lit colors for the particle.  The contribution from each light
        // is kept separate because the phase function we apply at runtime depends on the light vector
        // for each light source separately.  This view-dependent effect is impossible without knowing 
        // the amount of light contributed for each light.  This, of course, assumes the clouds will
        // be lit by a reasonably small number of lights (The sun plus some simulation of light reflected
        // from the sky and / or ground.) This technique works very well for simulating anisotropic 
        // illumination by skylight.
        if (isFirstLight)
        {
            it->SetBaseColor(CLOUD_AMBIENT_COLOR);  
            it->ClearLitColors();
            it->AddLitColor(vecColor);
        }
        else
        {
            it->AddLitColor(vecColor);
        }
    
        // the following computation (scaling of the scattered amount by the phase function) is done
        // after the lit color is stored so we don't add the scattering to this particle twice.
        vecScatteredAmount *= 1.5; // rayleigh scattering phase function for angle of zero or 180 = 1.5!
    
        // clamp the color
        if (vecScatteredAmount.x > 1) vecScatteredAmount.x = 1;
        if (vecScatteredAmount.y > 1) vecScatteredAmount.y = 1;
        if (vecScatteredAmount.z > 1) vecScatteredAmount.z = 1;
        vecScatteredAmount.w = 1 - TRANSPARENCY;

        // Draw the particle as a texture billboard.  Use the scattered light amount as the color to 
        // simulate forward scattering of light by this particle.
        /*glBegin(GL_QUADS);
        DrawQuad(vecParticlePos, cam.X * (*iter)->GetRadius(), cam.Y * (*iter)->GetRadius(), vecScatteredAmount);
        glEnd();*/
        it->Draw();

        //TODO: Comment
        
        //glutSwapBuffers(); // Uncomment this swap buffers to visualize cloud illumination computation.
    }
  
    // Note: here we could optionally store the current back buffer as a shadow image
    // to be projected from the light position onto the scene.  This way we can have clouds shadow
    // the environment.
  
    // restore matrix stack and viewport.
    /*glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(iOldVP[0], iOldVP[1], iOldVP[2], iOldVP[3]);*/
    //SDL_GL_SwapBuffers();
    //return SKYRESULT_OK; 
}
