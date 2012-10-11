#include "pgr.h"

#include "bunny.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

GLuint bunnyVBO, bunnyEBO;

int width, height;// Window size
float rx = 0.0f, ry = 0.0f, pz = -10.0f;

unsigned mode = 0;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

GLuint vertexShader, fragmentShader, shaderProgram;

GLuint positionAttrib, normalAttrib;
GLuint mvpUniform, mvUniform, mnUniform;

GLuint lightPositionUniform, shininessUniform, modeUniform;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void onInit()
{
    // Compile shaders
    vertexShader = compileShader(GL_VERTEX_SHADER, loadFile("lab02_02_shadertex.vs").c_str());
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, loadFile("lab02_02_shadertex.fs").c_str());

    // Link shaders
    shaderProgram = linkShader(2, vertexShader, fragmentShader);

    positionAttrib = glGetAttribLocation(shaderProgram, "position");
    normalAttrib = glGetAttribLocation(shaderProgram, "normal");

    mvpUniform = glGetUniformLocation(shaderProgram, "mvp");
    mvUniform = glGetUniformLocation(shaderProgram, "mv");
    mnUniform = glGetUniformLocation(shaderProgram, "mn");

    lightPositionUniform = glGetUniformLocation(shaderProgram, "lightPosition");
    shininessUniform = glGetUniformLocation(shaderProgram, "shininess");

    modeUniform = glGetUniformLocation(shaderProgram, "mode");

    // Copy bunny to graphics card
    glGenBuffers(1, &bunnyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bunnyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bunnyVertices), bunnyVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &bunnyEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bunny), bunny, GL_STATIC_DRAW);
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Calculate ModelViewProjection matrix

    glm::mat4 projection = glm::perspective(45.0f, (float)width/(float)height, 1.0f, 1000.0f);

    glm::mat4 mv = glm::rotate(
            glm::rotate(
                glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3(0, 0, pz)
                    ),
                ry, glm::vec3(1, 0, 0)
                ),
            rx, glm::vec3(0, 1, 0)
            );

    glm::mat4 mvp = projection*mv;

    glm::mat3 mn = glm::mat3(
            glm::rotate(
                glm::rotate(
                    glm::mat4(1.0f),
                    ry, glm::vec3(1, 0, 0)
                    ),
                rx, glm::vec3(0, 1, 0)
                )
            );

    glUseProgram(shaderProgram);

    // Set matrices
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp)); 
    glUniformMatrix4fv(mvUniform, 1, GL_FALSE, glm::value_ptr(mv)); 
    glUniformMatrix3fv(mnUniform, 1, GL_FALSE, glm::value_ptr(mn)); 

    glUniform3f(lightPositionUniform, 0.0f, 2.0f, 0.0f);
    glUniform1f(shininessUniform, 64.0f);

    glUniform1f(modeUniform, mode);

    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(normalAttrib);

    // Draw bunny
    glBindBuffer(GL_ARRAY_BUFFER, bunnyVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(BunnyVertex), (void*)offsetof(BunnyVertex, position));
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(BunnyVertex), (void*)offsetof(BunnyVertex, normal));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyEBO);
    glDrawElements(GL_TRIANGLES, sizeof(bunny)/sizeof(**bunny), bunnyIndexType, NULL);

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
        case SDLK_ESCAPE : quit(); break;
        case SDLK_1 : mode = 0; redraw(); break;
        case SDLK_2 : mode = 1; redraw(); break;
        case SDLK_3 : mode = 2; redraw(); break;
        case SDLK_4 : mode = 3; redraw(); break;
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

        mainLoop();

    } catch(exception & ex) {
        cout << "ERROR : " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
