#include "pgr.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Possible primitive colors
const float colors[][3] = {
    {1, 0, 0}, {0, 1, 0}, {0, 0, 1}
};

// Selected color
unsigned color = 0;

const float points[][2] = {
    {0,0.8}, {0.6,0.6}, {0.8, 0}, {0.6, -0.6},
    {0, -0.8}, {-0.6, -0.6}, {-0.8, 0}, {-0.6, 0.6}
};

GLuint pointsVBO;

// Primitive to draw
GLenum primitive = GL_POINTS;

// Point size, (line width)
float size = 1;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * vertexShaderSource
    = "#version 130\n in vec2 position; void main() { gl_Position = vec4(position,0,1); }";
const char * fragmentShaderSource
    = "#version 130\n uniform vec3 color; out vec4 fragColor; void main() { fragColor = vec4(color,1); }";

GLuint vertexShader, fragmentShader, shaderProgram;

GLuint positionAttrib;
GLuint colorUniform;

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

    // Get location of "color" uniform
    colorUniform = glGetUniformLocation(shaderProgram, "color");

    // Copy points to graphics card
    glGenBuffers(1, &pointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
}

// Called when window needs to be redrawn
void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glEnableVertexAttribArray(positionAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // Set selected color
    glUniform3fv(colorUniform, 1, colors[color]);

    // Set sizes
    glPointSize(size);
    glLineWidth(size);

    // Draw selected primitive
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
        case SDLK_ESCAPE : quit(); break;
        case SDLK_1 : primitive = GL_POINTS; break;
        case SDLK_2 : primitive = GL_LINES; break;
        case SDLK_3 : primitive = GL_LINE_STRIP; break;
        case SDLK_4 : primitive = GL_LINE_LOOP; break;
        case SDLK_5 : primitive = GL_TRIANGLES; break;
        case SDLK_6 : primitive = GL_TRIANGLE_STRIP; break;
        case SDLK_7 : primitive = GL_TRIANGLE_FAN; break;
        case SDLK_UP : if(color < (sizeof(colors)/sizeof(*colors) -1)) ++color; break;
        case SDLK_DOWN : if(color > 0) --color; break;
        case SDLK_KP_PLUS : size += 1; break;
        case SDLK_KP_MINUS : if(size > 1) size -= 1; break;
        default : break;
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
