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
    float texcoord[2];
    float position[3];
} const houseVertices[] = {
    // Walls
    { { 0.0, 0.0 }, { -5.0, -5.0, -5.0 } },
    { { 0.0, 4.0 }, { -5.0, -5.0,  5.0 } },
    { { 4.0, 4.0 }, {  5.0, -5.0,  5.0 } },
    { { 4.0, 0.0 }, {  5.0, -5.0, -5.0 } },
        
    { { 0.0, 0.0 }, { -5.0,  5.0, -5.0 } },
    { { 0.0, 2.0 }, { -5.0,  5.0,  5.0 } },
    { { 2.0, 2.0 }, {  5.0,  5.0,  5.0 } },
    { { 2.0, 0.0 }, {  5.0,  5.0, -5.0 } },
        
    { { 0.0, 0.0 }, { -5.0, -5.0, -5.0 } },
    { { 0.0, 1.0 }, { -5.0, -5.0,  5.0 } },
    { { 1.0, 1.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 0.0 }, { -5.0,  5.0, -5.0 } },
        
    { { 0.0, 0.0 }, {  5.0, -5.0, -5.0 } },
    { { 0.0, 1.0 }, {  5.0, -5.0,  5.0 } },
    { { 1.0, 1.0 }, {  5.0,  5.0,  5.0 } },
    { { 1.0, 0.0 }, {  5.0,  5.0, -5.0 } },
    // Roof
    { { 0.0, 0.0 }, { -5.0,  5.0, -5.0 } },
    { { 1.0, 0.0 }, {  5.0,  5.0, -5.0 } },
    { { 0.0, 1.0 }, {  0.0, 11.0,  0.0 } }, 

    { { 0.0, 0.0 }, {  5.0,  5.0, -5.0 } },
    { { 1.0, 0.0 }, {  5.0,  5.0,  5.0 } },
    { { 0.0, 1.0 }, {  0.0, 11.0,  0.0 } },

    { { 0.0, 0.0 }, {  5.0,  5.0,  5.0 } },
    { { 0.0, 1.0 }, { -5.0,  5.0,  5.0 } },
    { { 1.0, 1.0 }, {  0.0, 11.0,  0.0 } },

    { { 0.0, 1.0 }, { -5.0,  5.0,  5.0 } },
    { { 0.0, 0.0 }, { -5.0,  5.0, -5.0 } },
    { { 1.0, 1.0 }, {  0.0, 11.0,  0.0 } }
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

GLuint texture, compressedTexture;

int width, height;
float rx = 0.0f, ry = 0.0f, pz = -70.0f;

GLenum wrap = GL_REPEAT, magfilter = GL_NEAREST, minfilter = GL_NEAREST;
bool compressed = false;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * VSSource
    = "#version 130\n in vec3 position; in vec2 tc; uniform mat4 mvp; out vec2 coord; void main() { gl_Position = mvp*vec4(position,1); coord = tc; }";
const char * FSSource
    = "#version 130\n in vec2 coord; uniform sampler2D tex; out vec4 fragColor; void main() { fragColor = texture(tex, coord); }";

GLuint VS, FS, Prog;

GLuint positionAttrib, tcAttrib, mvpUniform;

GLuint textureUniform;

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
    tcAttrib = glGetAttribLocation(Prog, "tc");
    mvpUniform = glGetUniformLocation(Prog, "mvp");

    textureUniform = glGetUniformLocation(Prog, "tex");

    // Copy house to graphics card
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(houseVertices), houseVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(house), house, GL_STATIC_DRAW);

    //Load texture from file
    SDL_Surface * surface = SDL_LoadBMP("tiles.bmp");
    if(surface == NULL) throw SDL_Exception();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface);
    glGenerateMipmap(GL_TEXTURE_2D);

    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST);

    glGenTextures(1, &compressedTexture);
    glBindTexture(GL_TEXTURE_2D, compressedTexture);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, surface);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glUseProgram(Prog);

    //MVP
    glm::mat4 mvp = glm::rotate(
            glm::rotate(
                glm::translate(
                    glm::perspective(45.0f, (float)width/(float)height, 1.0f, 1000.0f),
                    glm::vec3(0, 0, pz)
                    ),
                ry, glm::vec3(1, 0, 0)
                ),
            rx, glm::vec3(0, 1, 0)
            );

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, compressed ? compressedTexture : texture);

    const float red[] = {1.0f, 0.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, red);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);

    glUniform1i(textureUniform, 0);

    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(tcAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
    glVertexAttribPointer(tcAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, texcoord));

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
        case SDLK_r : wrap = GL_REPEAT; redraw(); break;
        case SDLK_m : wrap = GL_MIRRORED_REPEAT; redraw(); break;
        case SDLK_c : wrap = GL_CLAMP; redraw(); break;
        case SDLK_b : wrap = GL_CLAMP_TO_BORDER; redraw(); break;
        case SDLK_e : wrap = GL_CLAMP_TO_EDGE; redraw(); break;
        case SDLK_t : compressed = !compressed; redraw(); break;
        case SDLK_1 : magfilter = minfilter = GL_NEAREST; redraw(); break;
        case SDLK_2 : magfilter = minfilter = GL_LINEAR; redraw(); break;
        case SDLK_3 : magfilter = GL_LINEAR; minfilter = GL_NEAREST_MIPMAP_NEAREST; redraw(); break;
        case SDLK_4 : magfilter = GL_LINEAR; minfilter = GL_NEAREST_MIPMAP_LINEAR; redraw(); break;
        case SDLK_5 : magfilter = GL_LINEAR; minfilter = GL_LINEAR_MIPMAP_NEAREST; redraw(); break;
        case SDLK_6 : magfilter = GL_LINEAR; minfilter = GL_LINEAR_MIPMAP_LINEAR; redraw(); break;
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
