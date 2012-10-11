#include "pgr.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Colored points
struct Point
{
    float position[2];
    float color[3];
} points[] = {
    {{ 0  ,  0.8}, {1, 0, 0}},
    {{ 0.6,  0.6}, {0, 1, 0}},
    {{ 0.8,  0  }, {0, 0, 1}},
    {{ 0.6, -0.6}, {1, 0, 0}},
    {{ 0  , -0.8}, {0, 1, 0}},
    {{-0.6, -0.6}, {0, 0, 1}},
    {{-0.8,  0  }, {1, 0, 0}},
    {{-0.6,  0.6}, {0, 1, 0}}
};

GLuint pointsVBO;

GLenum primitive = GL_POINTS, mode = GL_FILL;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * vertexShaderSource
    = "#version 130\n in vec2 position; in vec3 color; out vec3 c; void main() { gl_Position = vec4(position,0,1); c = color; }";
const char * fragmentShaderSource
    = "#version 130\n in vec3 c; out vec4 fragColor; void main() { fragColor = vec4(c,1); }";

GLuint vertexShader, fragmentShader, shaderProgram;

GLuint positionAttrib, colorAttrib;

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

    // Copy points to graphics card
    glGenBuffers(1, &pointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    // Enable both attributes
    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);

    // Set pointers - step and offset
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));

    // Set polygon rasterization mode
    glPolygonMode(GL_FRONT_AND_BACK, mode);

    glDrawArrays(primitive, 0, sizeof(points)/sizeof(*points));

    SDL_GL_SwapBuffers();
}

void onWindowResized(int width, int height)
{
    glViewport(0, 0, width, height);
}

void onKeyDown(SDLKey key, Uint16 /*mod*/)
{
    switch(key) {
        case SDLK_ESCAPE : quit(); return; break;
        case SDLK_1 : primitive = GL_POINTS; break;
        case SDLK_2 : primitive = GL_LINES; break;
        case SDLK_3 : primitive = GL_LINE_STRIP; break;
        case SDLK_4 : primitive = GL_LINE_LOOP; break;
        case SDLK_5 : primitive = GL_TRIANGLES; break;
        case SDLK_6 : primitive = GL_TRIANGLE_STRIP; break;
        case SDLK_7 : primitive = GL_TRIANGLE_FAN; break;
        case SDLK_p : mode = GL_POINT; break;
        case SDLK_l : mode = GL_LINE; break;
        case SDLK_f : mode = GL_FILL; break;
        default : return; break;
    }
    redraw();
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
