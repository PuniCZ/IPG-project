#include "pgr.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
const float M_PI = 3.14159;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Point {
    float color[4];
    float position[3];
} const houseVertices[] = {
    
    // Walls TOP
    { { 0.0, 0.0, 1.0, 1.0 }, { -5.0, -5.0, -5.0 } },
    { { 0.0, 1.0, 0.0, 1.0 }, { -5.0, -5.0,  5.0 } },
    { { 0.0, 1.0, 1.0, 1.0 }, {  5.0, -5.0,  5.0 } },
    { { 1.0, 0.0, 0.0, 1.0 }, {  5.0, -5.0, -5.0 } },
    // DOWN



    { { 1.0, 0.0, 1.0, 1.0 }, { -5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 0.0, 1.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 1.0, 1.0 }, {  5.0,  5.0,  5.0 } },
    { { 0.0, 0.0, 1.0, 1.0 }, {  5.0,  5.0, -5.0 } },
        
    { { 0.0, 1.0, 0.0, 1.0 }, { -5.0, -5.0, -5.0 } },
    { { 0.0, 1.0, 1.0, 1.0 }, { -5.0, -5.0,  5.0 } },
    { { 1.0, 0.0, 0.0, 1.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 0.0, 1.0, 1.0 }, { -5.0,  5.0, -5.0 } },
        
    { { 0.0, 1.0, 0.0, 1.0 }, {  5.0, -5.0, -5.0 } },
    { { 0.0, 1.0, 1.0, 1.0 }, {  5.0, -5.0,  5.0 } },
    { { 1.0, 0.0, 0.0, 1.0 }, {  5.0,  5.0,  5.0 } },
    { { 1.0, 0.0, 1.0, 1.0 }, {  5.0,  5.0, -5.0 } },
    // Roof
    { { 0.0, 0.0, 1.0, 1.0 }, { -5.0,  5.0, -5.0 } },
    { { 0.0, 1.0, 1.0, 1.0 }, {  5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } }, 

    { { 1.0, 0.0, 0.0, 1.0 }, {  5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 0.0, 1.0 }, {  5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } },

    { { 0.0, 1.0, 0.0, 1.0 }, {  5.0,  5.0,  5.0 } },
    { { 0.0, 1.0, 1.0, 1.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } },

    { { 0.0, 1.0, 0.0, 1.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0, 0.0, 1.0 }, { -5.0,  5.0, -5.0 } },
    { { 1.0, 1.0, 1.0, 1.0 }, {  0.0, 11.0,  0.0 } },

    // grass
    { { 0.2, 0.7, 0.0, 1.0 }, { -15.0, -5.1, -15 } },
    { { 0.2, 0.7, 0.0, 1.0 }, { 15.0, -5.1, -15  } },
    { { 0.2, 0.7, 0.0, 1.0 }, { -15.0, -5.1, 15  } },
    { { 0.2, 0.7, 0.0, 1.0 }, { 15.0, -5.1,  15  } },

    // window
    { { 0.2, 0.2, 1.0, 0.5 }, { -5.0, -5.0, -5.0 } },
    { { 0.2, 0.2, 1.0, 0.5 }, { -5.0,  5.0, -5.0 } },
    { { 0.2, 0.2, 1.0, 0.5 }, {  0.0,  5.0, -5.0 } },
    { { 0.2, 0.2, 1.0, 0.5 }, {  0.0, -5.0, -5.0 } },

    // window
    { { 0.2, 0.2, 1.0, 0.5 }, {  0.0, -5.0,  5.0 } },
    { { 0.2, 0.2, 1.0, 0.5 }, {  0.0,  5.0,  5.0 } },
    { { 0.2, 0.2, 1.0, 0.5 }, {  5.0,  5.0,  5.0 } },
    { { 0.2, 0.2, 1.0, 0.5 }, {  5.0, -5.0,  5.0 } },



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
    25, 26, 27,
    28, 29, 30,
    29, 30, 31,
//window
    32, 33, 34,
    32, 34, 35,
//window
    36, 37, 38,
    36, 38, 39,

};

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
GLuint aspectUniform, translateUniform, rotateUniform, scaleUniform;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void onInit()
{
    // Shader
    VS = compileShader(GL_VERTEX_SHADER, loadFile("lab01_task.vs").c_str());
    FS = compileShader(GL_FRAGMENT_SHADER, loadFile("lab01_task.fs").c_str());
    Prog = linkShader(2, VS, FS);

    positionAttrib = glGetAttribLocation(Prog, "position");
    colorAttrib = glGetAttribLocation(Prog, "color");

    aspectUniform = glGetUniformLocation(Prog, "aspect");
    translateUniform = glGetUniformLocation(Prog, "translate");
    rotateUniform = glGetUniformLocation(Prog, "rotate");
    scaleUniform = glGetUniformLocation(Prog, "scale");

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

    glEnable(GL_DEPTH_TEST);

    glUseProgram(Prog);

    glUniform1f(aspectUniform, (float)width/(float)height);
    glUniform1f(translateUniform, pz);
    glUniform2f(rotateUniform, ry*M_PI/180, rx*M_PI/180);
    glUniform1f(scaleUniform, wheel);

    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, sizeof(house)/sizeof(*house), GL_UNSIGNED_BYTE, NULL);

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
