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

struct Point {
    float color[3];
    float position[3];
} const houseVertices[] = {
    // Walls
    { { 0.0, 0.0, 1.0 }, { -5.0, -5.0, -5.0 } },
    { { 0.0, 1.0, 0.0 }, { -5.0, -5.0,  5.0 } },
    { { 0.0, 1.0, 1.0 }, {  5.0, -5.0,  5.0 } },
    { { 1.0, 0.0, 0.0 }, {  5.0, -5.0, -5.0 } },
        
    { { 1.0, 0.0, 1.0 }, { -5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 0.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 1.0 }, {  5.0,  5.0,  5.0 } },
    { { 0.0, 0.0, 1.0 }, {  5.0,  5.0, -5.0 } },
        
    { { 0.0, 1.0, 0.0 }, { -5.0, -5.0, -5.0 } },
    { { 0.0, 1.0, 1.0 }, { -5.0, -5.0,  5.0 } },
    { { 1.0, 0.0, 0.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 0.0, 1.0 }, { -5.0,  5.0, -5.0 } },
        
    { { 0.0, 1.0, 0.0 }, {  5.0, -5.0, -5.0 } },
    { { 0.0, 1.0, 1.0 }, {  5.0, -5.0,  5.0 } },
    { { 1.0, 0.0, 0.0 }, {  5.0,  5.0,  5.0 } },
    { { 1.0, 0.0, 1.0 }, {  5.0,  5.0, -5.0 } },
    // Roof
    { { 0.0, 0.0, 1.0 }, { -5.0,  5.0, -5.0 } },
    { { 0.0, 1.0, 1.0 }, {  5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } }, 

    { { 1.0, 0.0, 0.0 }, {  5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 0.0 }, {  5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } },

    { { 0.0, 1.0, 0.0 }, {  5.0,  5.0,  5.0 } },
    { { 0.0, 1.0, 1.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } },

    { { 0.0, 1.0, 0.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 0.0 }, { -5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } }
};

// House indices
const unsigned char house[] = {
//Walls
     0,  1,  2,
     0,  2,  3,
     4,  5,  6,
     4,  6,  7,
     8,  9, 10,
     8, 10, 11,
    12, 13, 14,
    12, 14, 15,
//Roof
    16, 17, 18,
    19, 20, 21,
    22, 23, 24,
    25, 26, 27
};

GLuint VBO, EBO;

enum Mode {
    ORTHOGONAL, PERSPECTIVE
} mode = ORTHOGONAL;

int width, height;
float rx = 0.0f, ry = 0.0f, pz = -70.0f;

float wheel = 1.0f;
bool depth = false;

GLenum func = GL_LESS;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * VSSource
    = "#version 130\n in vec3 position; in vec3 color; uniform mat4 mvp; out vec3 c; void main() { gl_Position = mvp*vec4(position,1); c = color; }";
const char * FSSource
    = "#version 130\n in vec3 c; out vec4 fragColor; void main() { fragColor = vec4(c,1); }";

GLuint VS, FS, Prog;

GLuint positionAttrib, colorAttrib, mvpUniform;

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
    colorAttrib = glGetAttribLocation(Prog, "color");
    mvpUniform = glGetUniformLocation(Prog, "mvp");

    // Copy house to graphics card
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(houseVertices), houseVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(house), house, GL_STATIC_DRAW);
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(depth)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    glDepthFunc(func);

    glUseProgram(Prog);

    //MVP
    glm::mat4 projection;

    float aspect = (float)width/(float)height;

    if(mode == ORTHOGONAL)
    {
        projection = glm::ortho(-wheel*aspect, wheel*aspect, -wheel, wheel, 1.0f, 1000.0f);
    }
    else
    {
        projection = glm::perspective(10.0f*wheel, aspect, 1.0f, 1000.0f);
    }

    glm::mat4 mvp = glm::rotate(
            glm::rotate(
                glm::translate(
                    projection,
                    glm::vec3(0, 0, pz)
                    ),
                ry, glm::vec3(1, 0, 0)
                ),
            rx, glm::vec3(0, 1, 0)
            );

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, sizeof(house)/sizeof(*house), GL_UNSIGNED_BYTE, NULL);

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
        case SDLK_o : mode = ORTHOGONAL; break;
        case SDLK_p : mode = PERSPECTIVE; break;
        case SDLK_d : depth = !depth; break;
        case SDLK_1 : func = GL_NEVER; break;
        case SDLK_2 : func = GL_ALWAYS; break;
        case SDLK_3 : func = GL_LESS; break;
        case SDLK_4 : func = GL_LEQUAL; break;
        case SDLK_5 : func = GL_EQUAL; break;
        case SDLK_6 : func = GL_GREATER; break;
        case SDLK_7 : func = GL_GEQUAL; break;
        case SDLK_8 : func = GL_NOTEQUAL; break;
        default : return;
    }
    redraw();
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
        case SDL_BUTTON_WHEELDOWN : wheel -= 1;/* if(wheel < 1) wheel = 1; */break;
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
