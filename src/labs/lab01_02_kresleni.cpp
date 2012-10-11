#include "pgr.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Positions of 8 points
const float points[][2] = {
    {0,0.8}, {0.6,0.6}, {0.8, 0}, {0.6, -0.6},
    {0, -0.8}, {-0.6, -0.6}, {-0.8, 0}, {-0.6, 0.6}
};

GLuint pointsVBO;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * vertexShaderSource
    = "#version 130\n in vec2 position; void main() { gl_Position = vec4(position,0,1); }";
const char * fragmentShaderSource
    = "#version 130\n out vec4 fragColor; void main() { fragColor = vec4(1); }";

GLuint vertexShader, fragmentShader, shaderProgram;

GLuint positionAttrib;

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

    // Get location of the "position" attribute
    positionAttrib = glGetAttribLocation(shaderProgram, "position");

    // Copy points to graphics card
    glGenBuffers(1, &pointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
}

// Called when window needs to be redrawn
void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Activate shader
    glUseProgram(shaderProgram);

    // Enable position attribute
    glEnableVertexAttribArray(positionAttrib);

    // Bind VBO and set pointer
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // Draw points
    glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(points)/sizeof(*points));

    SDL_GL_SwapBuffers();
}

void onWindowResized(int width, int height)
{
    glViewport(0, 0, width, height);
}

void onKeyDown(SDLKey key, Uint16 /*mod*/)
{
    switch(key) {
        case SDLK_ESCAPE : quit(); break;
        default : break;//nothing-doing defaut to shut up warning
    }
}

void onKeyUp(SDLKey /*key*/, Uint16 /*mod*/)
{
}

void onMouseMove(unsigned /*x*/, unsigned /*y*/, int /*xrel*/, int /*yrel*/, Uint8 /*buttons*/)
{
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
