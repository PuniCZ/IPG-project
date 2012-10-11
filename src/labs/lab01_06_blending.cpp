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
    float position[2];
    float color[3];
} const triangle[] = {
    { {-1, -1}, {1, 0, 0} },
    { { 1, -1}, {0, 1, 0} },
    { { 0,  1}, {0, 0, 1} }
};

const float quad[][2] = {
    { -1, -1 },
    {  1, -1 },
    {  1,  1 },
    { -1,  1 }
};

const unsigned N = 4;

const glm::vec3 quadPositions[N] = {
    glm::vec3(200, 150, 0), glm::vec3(600, 150, 0), glm::vec3(600, 450, 0), glm::vec3(200, 450, 0)
};

const float quadColors[N][3] = {
    { 0, 0, 1 }, { 0, 1, 0 }, { 1, 0, 0 }, { 0, 1, 0 }
};

GLuint triVBO, quadVBO;

int width, height;
float px = 400, py = 200;

// Blending factors
GLenum sfactor = GL_ONE, dfactor = GL_ZERO, equation = GL_FUNC_ADD;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * triVSSource
    = "#version 130\n in vec2 position; in vec3 color; uniform mat4 mvp; out vec3 c; void main() { gl_Position = mvp*vec4(position,0,1); c = color; }";
const char * triFSSource
    = "#version 130\n in vec3 c; out vec4 fragColor; void main() { fragColor = vec4(c,1); }";

const char * quadVSSource
    = "#version 130\n in vec2 position; uniform vec3 color; uniform mat4 mvp; out vec4 c; void main() { gl_Position = mvp*vec4(position,0,1); c = vec4(color, position.x); }";
const char * quadFSSource
    = "#version 130\n in vec4 c; out vec4 fragColor; void main() { fragColor = c; }";

GLuint triVS, triFS, triP;
GLuint quadVS, quadFS, quadP;

GLuint triPos, triColor, triMVP;
GLuint quadPos, quadColor, quadMVP;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void onInit()
{
    // Shader for triangle
    triVS = compileShader(GL_VERTEX_SHADER, triVSSource);
    triFS = compileShader(GL_FRAGMENT_SHADER, triFSSource);
    triP = linkShader(2, triVS, triFS);

    triPos = glGetAttribLocation(triP, "position");
    triColor = glGetAttribLocation(triP, "color");
    triMVP = glGetUniformLocation(triP, "mvp");

    // Shader for quad
    quadVS = compileShader(GL_VERTEX_SHADER, quadVSSource);
    quadFS = compileShader(GL_FRAGMENT_SHADER, quadFSSource);
    quadP = linkShader(2, quadVS, quadFS);

    quadPos = glGetAttribLocation(quadP, "position");
    quadColor = glGetUniformLocation(quadP, "color");
    quadMVP = glGetUniformLocation(quadP, "mvp");

    // Copy triangle to graphics card
    glGenBuffers(1, &triVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    // Copy quad to graphics card
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);

    //draw triangle

    glUseProgram(triP);

    glm::mat4 model= glm::scale(
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(px, height-py, 0)
                ),
            glm::vec3(100, 100, 1)
            );

    glm::mat4 mvp = projection*model;

    glUniformMatrix4fv(triMVP, 1, GL_FALSE, glm::value_ptr(mvp)); 

    glEnableVertexAttribArray(triPos);
    glEnableVertexAttribArray(triColor);

    glBindBuffer(GL_ARRAY_BUFFER, triVBO);
    glVertexAttribPointer(triPos, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
    glVertexAttribPointer(triColor, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));

    glDrawArrays(GL_TRIANGLES, 0, sizeof(triangle)/sizeof(*triangle));

    // Disable vertex attributes of triangle
    glDisableVertexAttribArray(triPos);
    glDisableVertexAttribArray(triColor);

    // Setup blending
    
    glEnable(GL_BLEND);
    glBlendFunc(sfactor, dfactor);
    glBlendEquation(equation);

    // Draw quads

    glUseProgram(quadP);

    glEnableVertexAttribArray(quadPos);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glVertexAttribPointer(quadPos, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    for(unsigned i = 0; i < N; ++i)
    {
        glUniform3fv(quadColor, 1, quadColors[i]);
        
        glm::mat4 model = glm::scale(
                glm::translate(
                    glm::mat4(1.0f),
                    quadPositions[i]
                    ),
                glm::vec3(100, 100, 1)
                );

        glm::mat4 mvp = projection*model;
        
        glUniformMatrix4fv(quadMVP, 1, GL_FALSE, glm::value_ptr(mvp));

        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(quad)/sizeof(*quad));
    }

    // Disable vertex attributes of quad
    glDisableVertexAttribArray(quadPos);

    glDisable(GL_BLEND);

    SDL_GL_SwapBuffers();
}

void onWindowResized(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w; height = h;
}

void onKeyDown(SDLKey key, Uint16 mod)
{
    if(mod & KMOD_CTRL)
    {
        switch(key) {
            case SDLK_1 : equation = GL_FUNC_ADD; break;
            case SDLK_2 : equation = GL_FUNC_SUBTRACT; break;
            case SDLK_3 : equation = GL_FUNC_REVERSE_SUBTRACT; break;
            case SDLK_4 : equation = GL_MIN; break;
            case SDLK_5 : equation = GL_MAX; break;
            default : return;
        }
    }
    else
    {
        GLenum & factor = mod & KMOD_SHIFT ? dfactor : sfactor;
        switch(key) {
            case SDLK_ESCAPE : quit(); return;
            case SDLK_1 : factor = GL_ZERO; break;
            case SDLK_2 : factor = GL_ONE; break;
            case SDLK_3 : factor = GL_DST_COLOR; break;
            case SDLK_4 : factor = GL_ONE_MINUS_DST_COLOR; break;
            case SDLK_5 : factor = GL_SRC_ALPHA; break;
            case SDLK_6 : factor = GL_ONE_MINUS_SRC_ALPHA; break;
            case SDLK_7 : factor = GL_DST_ALPHA; break;
            case SDLK_8 : factor = GL_ONE_MINUS_DST_ALPHA; break;
            case SDLK_9 : factor = GL_SRC_ALPHA_SATURATE; break;
            default : return;
        }
    }
    redraw();
}

void onKeyUp(SDLKey /*key*/, Uint16 /*mod*/)
{
}

void onMouseMove(unsigned x, unsigned y, int /*xrel*/, int /*yrel*/, Uint8 buttons)
{
    if(buttons & SDL_BUTTON_LMASK)
    {
        px = x;
        py = y;
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
