#include "pgr.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Cloud.h"
#include "Utils.h"

using namespace std;
const float M_PI = 3.141592654f;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



GLuint VBO, EBO;

int width, height;
float rx = 0.0f, ry = 0.0f, pz = -70.0f, wheel = 1.0f;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

GLuint VS, FS, Prog;
GLuint texture;

GLuint positionAttrib, colorAttrib, texCoordsAtrib;
GLuint rotateUniform;
GLuint mvpUniform;

GLuint textureUniform;

std::vector<Cloud*> clouds;
int numOfClouds = 2;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void onInit()
{
    // Shader
    VS = compileShader(GL_VERTEX_SHADER, loadFile("particle.vs").c_str());
    FS = compileShader(GL_FRAGMENT_SHADER, loadFile("particle.fs").c_str());
    Prog = linkShader(2, VS, FS);

    positionAttrib = glGetAttribLocation(Prog, "position");
    colorAttrib = glGetAttribLocation(Prog, "color");
    texCoordsAtrib = glGetAttribLocation(Prog, "texCoordIn");

    textureUniform = glGetUniformLocation(Prog, "tex");
    rotateUniform = glGetUniformLocation(Prog, "rotate");

    mvpUniform = glGetUniformLocation(Prog, "mvp");

    //Load texture from file
    /*SDL_Surface * surface = SDL_LoadBMP("particle.bmp");
    if(surface == NULL) throw SDL_Exception();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface);
    glGenerateMipmap(GL_TEXTURE_2D);*/


    clouds.push_back(new Cloud(glm::vec3(0), glm::vec3(7, 2, 5)));
    clouds.push_back(new Cloud(glm::vec3(6, 5, 2), glm::vec3(4, 1.5f, 3)));
    clouds.push_back(new Cloud(glm::vec3(-6, -5, -10), glm::vec3(4, 1.5f, 3)));

    clouds.push_back(new Cloud(glm::vec3(50, 5, -5), glm::vec3(8, 8, 6)));
    clouds.push_back(new Cloud(glm::vec3(56, 15, -6), glm::vec3(16, 5, 9)));
    clouds.push_back(new Cloud(glm::vec3(44, 10, -9), glm::vec3(12, 5, 8)));

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*36*Cloud::particlesInCloud*clouds.size(), NULL, GL_STREAM_DRAW);
    int i = 0;
    for (std::vector<Cloud*>::iterator it = clouds.begin(); it != clouds.end(); it++)
    {
        //glm::vec3(0), glm::vec3(7, 2, 5)
        (*it)->CopyParticlesToBuffer(VBO, i*36*Cloud::particlesInCloud);
        i++;
    }


    texture = Utils::CreateSplatTexture(32);

    
    

    /*
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(houseVertices), houseVertices, GL_STATIC_DRAW);*/

    /*glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(house), house, GL_STATIC_DRAW);*/
}

int nomalizeAngle(float angle)
{
    return int(angle)%360 < 0 ? int(angle)%360 + 360 : int(angle)%360;
}

void onWindowRedraw()
{
    //glClearColor(0.f, 0.53f, 1.f, 1.f); 

    glClearColor(0.53f, 0.73f, 1.f, 1.f); 

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    

    glUseProgram(Prog);

    // Calculate MVP matrix
    glm::mat4 projection = glm::perspective(45.0f, (float)width/(float)height, 1.0f, 1000.0f);
    
    glm::mat4 mv = glm::scale(
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
            );
    
    /*glm::mat4 mv = glm::scale(
                    glm::translate(
                        glm::mat4(1.0f),
                        glm::vec3(0, 0, pz)
                        ),
                glm::vec3(min(pow(1.5f, wheel), 50.0f))
            );*/

    glm::mat4 mvp = projection * mv;


//    glUniformMatrix4fv(rotateUniform, 1, GL_FALSE, glm::value_ptr(mv2)); 

    // Set up matrices
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp)); 

    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);
    glEnableVertexAttribArray(texCoordsAtrib);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureUniform, 0); 

    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 36, (void*)0);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 36, (void*)16);
    glVertexAttribPointer(texCoordsAtrib, 2, GL_FLOAT, GL_FALSE, 36, (void*)28);

    glDepthMask(GL_FALSE); 
    glDrawArrays(GL_QUADS, 0, clouds.size() * Cloud::particlesInCloud * 4);
    glDepthMask(GL_TRUE);


    
    
    glDisable(GL_BLEND);

    SDL_GL_SwapBuffers();
}

void onWindowResized(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w; height = h;
}

void onKeyDown(SDLKey key, Uint16 /*mod*/)
{
    switch(key) {
        case SDLK_ESCAPE : quit(); return;
        default : return;
    }
}

void onKeyUp(SDLKey /*key*/, Uint16 /*mod*/)
{
}

void onMouseMove(unsigned /*x*/, unsigned /*y*/, int xrel, int yrel, Uint8 buttons)
{
    if(buttons & SDL_BUTTON_LMASK)
    {
        rx += xrel;
        ry += yrel;
        redraw();
    }
    if(buttons & SDL_BUTTON_RMASK)
    {
        pz += yrel;
        redraw();
    }
}

void onMouseDown(Uint8 button, unsigned /*x*/, unsigned /*y*/)
{
    switch(button) {
        case SDL_BUTTON_WHEELUP : wheel += 1; break;
        case SDL_BUTTON_WHEELDOWN : wheel -= 1; break;
        default : return;
    };
    redraw();
}

void onMouseUp(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/)
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int /*argc*/, char ** /*argv*/)
{
    try {
        // Init SDL - only video subsystem will be used
        if(SDL_Init(SDL_INIT_VIDEO) < 0) throw SDL_Exception();

        // Shutdown SDL when program ends
        atexit(SDL_Quit);

        init(800, 600, 24, 16, 0);

        mainLoop();

    } catch(exception & ex) {
        cout << "ERROR : " << ex.what() << endl;
        getchar();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
