#include "pgr.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Cloud.h"

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

GLuint positionAttrib, colorAttrib;
GLuint rotateUniform;
GLuint mvpUniform;

Cloud cloud;

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

    rotateUniform = glGetUniformLocation(Prog, "rotate");

    mvpUniform = glGetUniformLocation(Prog, "mvp");

    // Copy house to graphics card
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    cloud.CopyParticlesToBuffer(VBO, EBO);

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

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 28, (void*)16);
    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 28, (void*)0);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawArrays(GL_QUADS, 0, cloud.GetNumberOfParticles()*4);
    //glDrawElements(GL_TRIANGLES, cloud.GetNumberOfParticles()*6*4, GL_UNSIGNED_INT, NULL);


    
    
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
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
