#include "pgr.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const float quad[][2] = {
    { -1, -1 }, { 1, -1}, { 1, 1}, { -1, 1}
};

GLuint VBO;

struct Quad
{
    glm::vec3 position;
    float color[4];
} const quads[] = {
    { glm::vec3(0, 0,  10), {   1,   0,   0, 0.5 } },
    { glm::vec3(0, 0,   5), { 0.5, 0.5,   0, 0.5 } },
    { glm::vec3(0, 0,   0), {   0,   1,   0, 0.5 } },
    { glm::vec3(0, 0,  -5), {   0, 0.5, 0.5, 0.5 } },
    { glm::vec3(0, 0, -10), {   0,   0,   1, 0.5 } }
};

int width, height;
float rx = 0.0f, ry = 0.0f, pz = -70.0f;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * VSSource
    = "#version 130\n in vec2 position; uniform mat4 mvp; void main() { gl_Position = mvp*vec4(position,0,1); }";
const char * FSSource
    = "#version 130\n uniform vec4 color; out vec4 fragColor; void main() { fragColor = color; }";

GLuint VS, FS, Prog;

GLuint positionAttrib, colorUniform, mvpUniform;

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
    colorUniform = glGetUniformLocation(Prog, "color");
    mvpUniform = glGetUniformLocation(Prog, "mvp");

    // Copy house to graphics card
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //ViewProjection
    glm::mat4 vp = glm::rotate(
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

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    for(unsigned i = 0; i < sizeof(quads)/sizeof(*quads); ++i)
    {
        //ModelViewProjection
        glm::mat4 mvp = glm::translate(vp, quads[i].position);
        glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

        glUniform4fv(colorUniform, 1, quads[i].color);

        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(quad)/sizeof(*quad));
    }

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

        init(800, 600, 24, 16, 0);

        mainLoop();

    } catch(exception & ex) {
        cout << "ERROR : " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
