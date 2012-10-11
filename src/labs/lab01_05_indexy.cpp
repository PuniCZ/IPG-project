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

float houseVertices[][2] = {
    {-0.5, -0.5},
    {-0.5, 0.5},
    {0, 1},
    {0.5, 0.5},
    {0.5, -0.5}
};

float houseColors[][3] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    {1, 0, 0},
    {0, 1, 0}
};

unsigned char houseElements[] = { 0, 1, 2, 3, 4, 0, 3, 1, 4 };

GLuint houseVBO, houseVBOcolor, houseEBO;

int width, height;// Window size
float px = 400, py = 200, r = 0;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * vertexShaderSource
    = "#version 130\n in vec2 position; in vec3 color; uniform mat4 mvp; out vec3 c; void main() { gl_Position = mvp*vec4(position,0,1); c = color; }";
const char * fragmentShaderSource
    = "#version 130\n in vec3 c; out vec4 fragColor; void main() { fragColor = vec4(c,1); }";

GLuint vertexShader, fragmentShader, shaderProgram;

GLuint positionAttrib, colorAttrib;
GLuint mvpUniform;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void onInit()
{
    // Compile shaders
    vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Link shaders
    shaderProgram = linkShader(2, vertexShader, fragmentShader);

    positionAttrib = glGetAttribLocation(shaderProgram, "position");
    colorAttrib = glGetAttribLocation(shaderProgram, "color");
    mvpUniform = glGetUniformLocation(shaderProgram, "mvp");

    // Copy house to graphics card
    glGenBuffers(1, &houseVBO);
    glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(houseVertices), houseVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &houseVBOcolor);
    glBindBuffer(GL_ARRAY_BUFFER, houseVBOcolor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(houseColors), houseColors, GL_STATIC_DRAW);

    glGenBuffers(1, &houseEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, houseEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(houseElements), houseElements, GL_STATIC_DRAW);
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate ModelViewProjection matrix

    glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);

    glm::mat4 model = glm::scale(
            glm::rotate(
                glm::translate(
                    glm::mat4(1.0f),// Identity matrix
                    glm::vec3(px, height-py, 0)
                    ),
                r, glm::vec3(0, 0, 1)
                ),
            glm::vec3(100, 100, 1)
            );

    glm::mat4 mvp = projection*model;

    glUseProgram(shaderProgram);

    // Set MVP matrix
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp)); 

    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    // Set separately both attributes
    glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, houseVBOcolor);
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Bind index buffer and draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, houseEBO);
    glDrawElements(GL_LINE_STRIP, sizeof(houseElements)/sizeof(*houseElements), GL_UNSIGNED_BYTE, NULL);

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

void onMouseMove(unsigned x, unsigned y, int /*xrel*/, int yrel, Uint8 buttons)
{
    if(buttons & SDL_BUTTON_LMASK)
    {
        px = x;
        py = y;
        redraw();
    }
    if(buttons & SDL_BUTTON_RMASK)
    {
        r += yrel;
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

        init(800, 600, 24, 0, 0);

        mainLoop();

    } catch(exception & ex) {
        cout << "ERROR : " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
