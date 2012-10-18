#include "pgr.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
const float M_PI = 3.141592654f;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Point {
    float color[4];
    float position[3];
} const houseVertices[] = {
    
    // Walls
    { { 0.0f, 0.0f, 1.0f, 1.0f }, { -5.0f, -5.0f, -5.0f } },
    { { 0.0f, 1.0f, 0.0f, 1.0f }, { -5.0f, -5.0f,  5.0f } },
    { { 0.0f, 1.0f, 1.0f, 1.0f }, {  5.0f, -5.0f,  5.0f } },
    { { 1.0f, 0.0f, 0.0f, 1.0f }, {  5.0f, -5.0f, -5.0f } },

    { { 1.0f, 0.0f, 1.0f, 1.0f }, { -5.0f,  5.0f, -5.0f } },
    { { 1.0f, 1.0f, 0.0f, 1.0f }, { -5.0f,  5.0f,  5.0f } },
    { { 1.0f, 1.0f, 1.0f, 1.0f }, {  5.0f,  5.0f,  5.0f } },
    { { 0.0f, 0.0f, 1.0f, 1.0f }, {  5.0f,  5.0f, -5.0f } },
        
    { { 0.0f, 1.0f, 0.0f, 1.0f }, { -5.0f, -5.0f, -5.0f } },
    { { 0.0f, 1.0f, 1.0f, 1.0f }, { -5.0f, -5.0f,  5.0f } },
    { { 1.0f, 0.0f, 0.0f, 1.0f }, { -5.0f,  5.0f,  5.0f } },
    { { 1.0f, 0.0f, 1.0f, 1.0f }, { -5.0f,  5.0f, -5.0f } },
        
    { { 0.0f, 1.0f, 0.0f, 1.0f }, {  5.0f, -5.0f, -5.0f } },
    { { 0.0f, 1.0f, 1.0f, 1.0f }, {  5.0f, -5.0f,  5.0f } },
    { { 1.0f, 0.0f, 0.0f, 1.0f }, {  5.0f,  5.0f,  5.0f } },
    { { 1.0f, 0.0f, 1.0f, 1.0f }, {  5.0f,  5.0f, -5.0f } },

    // Roof
    { { 0.0f, 0.0f, 1.0f, 1.0f }, { -5.0f,  5.0f, -5.0f } },
    { { 0.0f, 1.0f, 1.0f, 1.0f }, {  5.0f,  5.0f, -5.0f } },
    { { 1.0f, 1.0f, 1.0f, 1.0f }, {  0.0f, 11.0f,  0.0f } }, 

    { { 1.0f, 0.0f, 0.0f, 1.0f }, {  5.0f,  5.0f, -5.0f } },
    { { 1.0f, 1.0f, 0.0f, 1.0f }, {  5.0f,  5.0f,  5.0f } },
    { { 1.0f, 1.0f, 1.0f, 1.0f }, {  0.0f, 11.0f,  0.0f } },

    { { 0.0f, 1.0f, 0.0f, 1.0f }, {  5.0f,  5.0f,  5.0f } },
    { { 0.0f, 1.0f, 1.0f, 1.0f }, { -5.0f,  5.0f,  5.0f } },
    { { 1.0f, 1.0f, 1.0f, 1.0f }, {  0.0f, 11.0f,  0.0f } },

    { { 0.0f, 1.0f, 0.0f, 1.0f }, { -5.0f,  5.0f,  5.0f } },
    { { 1.0f, 1.0f, 0.0f, 1.0f }, { -5.0f,  5.0f, -5.0f } },
    { { 1.0f, 1.0f, 1.0f, 1.0f }, {  0.0f, 11.0f,  0.0f } },

    // Grass floor
    { { 0.25f, 0.6f, 0.0f, 1.0f }, { -15.0f, -5.001f, -15.0f } },
    { { 0.25f, 0.6f, 0.0f, 1.0f }, {  15.0f, -5.001f, -15.0f  } },
    { { 0.25f, 0.6f, 0.0f, 1.0f }, { -15.0f, -5.001f,  15.0f  } },
    { { 0.25f, 0.6f, 0.0f, 1.0f }, {  15.0f, -5.001f,  15.0f  } },

    // Window #1
    { { 0.4f, 0.4f, 0.6f, 0.5f }, { -5.0f, -5.0f, -5.0f } },
    { { 0.4f, 0.4f, 0.6f, 0.5f }, { -5.0f,  5.0f, -5.0f } },
    { { 0.4f, 0.4f, 0.6f, 0.5f }, {  0.0f,  5.0f, -5.0f } },
    { { 0.4f, 0.4f, 0.6f, 0.5f }, {  0.0f, -5.0f, -5.0f } },

    // Window #2
    { { 0.4f, 0.4f, 0.6f, 0.5f }, {  0.0f, -5.0f,  5.0f } },
    { { 0.4f, 0.4f, 0.6f, 0.5f }, {  0.0f,  5.0f,  5.0f } },
    { { 0.4f, 0.4f, 0.6f, 0.5f }, {  5.0f,  5.0f,  5.0f } },
    { { 0.4f, 0.4f, 0.6f, 0.5f }, {  5.0f, -5.0f,  5.0f } },



};

// House indices
const unsigned char house[] = {
// Walls
     0,  1,  2, //0
     0,  2,  3,
     4,  5,  6,
     4,  6,  7,
     8,  9, 10,
     8, 10, 11,
    12, 13, 14,
    12, 14, 15,
// Roof
    16, 17, 18, //8
    19, 20, 21,
    22, 23, 24,
    25, 26, 27,
// Grass floor
    28, 29, 30, //12
    29, 30, 31,

// Window #1
    32, 33, 34, //14
    32, 34, 35,

// Window #2
    36, 37, 38, //16
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
GLuint mvpUniform;

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

    mvpUniform = glGetUniformLocation(Prog, "mvp");

    // Copy house to graphics card
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(houseVertices), houseVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(house), house, GL_STATIC_DRAW);
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

    glm::mat4 mvp = projection * mv;

    // Set up matrices
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp)); 

    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, sizeof(house)/sizeof(*house)-4*3, GL_UNSIGNED_BYTE, NULL);

    //windows (flip draw order if looking from "other side")
    bool flip = false;
    flip = (nomalizeAngle(rx) < 90 || nomalizeAngle(rx) > 270);
    if (!(nomalizeAngle(ry) < 90 || nomalizeAngle(ry) > 270))
        flip=!flip;   
    
    if (flip)
    {
        glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_BYTE, (void*)(14*3));
        glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_BYTE, (void*)(16*3));
    }
    else
    {
        glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_BYTE, (void*)(16*3));
        glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_BYTE, (void*)(14*3));
    }
    
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
