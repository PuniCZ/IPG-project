#include "pgr.h"

#include "sphere.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const float ground[][3] = {
    { -10, 0, 10 }, { 10, 0, 10 }, { 10, 0, -10 }, { -10, 0, -10 } };

GLuint sphereVBO, sphereEBO;
GLuint groundVBO;

const glm::vec3 position0(0, 10, 0), speed0(0.1, 0, 0);

glm::vec3 position = position0, speed = speed0;

int width, height;
float rx = 0.0f, ry = 0.0f, pz = -50.0f;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * VSSource
    = "#version 130\n in vec3 position; uniform mat4 mvp; void main() { gl_Position = mvp*vec4(position,1); }";
const char * FSSource
    = "#version 130\n uniform vec3 color; out vec4 fragColor; void main() { fragColor = vec4(color,1); }";

GLuint VS, FS, Prog;

GLuint positionAttrib, mvpUniform, colorUniform;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void onInit()
{
    // Shader
    VS = compileShader(GL_VERTEX_SHADER, VSSource);
    FS = compileShader(GL_FRAGMENT_SHADER, FSSource);
    Prog = linkShader(2, VS, FS);

    positionAttrib = glGetAttribLocation(Prog, "position");
    mvpUniform = glGetUniformLocation(Prog, "mvp");
    colorUniform = glGetUniformLocation(Prog, "color");

    glGenBuffers(1, &sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &sphereEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere), sphere, GL_STATIC_DRAW);

    glGenBuffers(1, &groundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);

}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    // t - absolute time, dt - time step
    static float t0 = SDL_GetTicks()/1000.0f;
    static float t = t0;
    float t1 = SDL_GetTicks()/1000.0f;
    float dt = t1-t;
    t = t1-t0;

    // Simulation step
    position += speed*dt;
    speed += glm::vec3(0.0f, -dt, 0.0f);//g*dt
    // Collision detection
    if(position.y < 0.0f)
    {
        // Bounce
        speed = glm::vec3(speed.x, -speed.y, speed.z)*0.8f;
        position.y = -position.y;
    }

    //MVP
    glm::mat4 vp = 
            glm::rotate(
                glm::rotate(
                    glm::translate(
                        glm::perspective(45.0f, (float)width/(float)height, 1.0f, 1000.0f),
                        glm::vec3(0, 0, pz)
                        ),
                    ry, glm::vec3(1, 0, 0)
                    ),
                rx, glm::vec3(0, 1, 0)
                );

    glUseProgram(Prog);

    glEnableVertexAttribArray(positionAttrib);

    glm::mat4 mvp = glm::translate(
            vp,
            position
            );

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

    glUniform3f(colorUniform, 1, 0, 0);

    // Draw sphere
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, position));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glDrawElements(GL_TRIANGLES, sizeof(sphere)/sizeof(*sphere)*3, sphereIndexType, NULL);

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(vp));

    glUniform3f(colorUniform, 0, 1, 0);

    // Draw floor
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(ground)/sizeof(*ground));

    SDL_GL_SwapBuffers();
}

void onWindowResized(int w, int h)
{
    width = w; height = h;
    glViewport(0, 0, w, h);
}

void onKeyDown(SDLKey key, Uint16 /*mod*/)
{
    switch(key) {
        case SDLK_ESCAPE : quit(); break;
        case SDLK_SPACE : position = position0; speed = speed0; break;
        default : break;//nothing-doing defaut to shut up warning
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

void onMouseDown(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/)
{
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

        init(800, 600, 24, 24, 0);

        mainLoop(40);//40ms == 25fps

    } catch(exception & ex) {
        cout << "ERROR : " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
