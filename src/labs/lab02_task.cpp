#include "pgr.h"

#include "sphere.h"

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

const float lightquad[][3] = {
    { -2.2, 4.97, -0.6},
    {-2.2, 4.97,  0.6},
    { 2.2, 4.97,  0.6},
    { 2.2, 4.97, -0.6}
};

GLuint HouseVBO, HouseEBO;

GLuint SphereVBO, SphereEBO;

GLuint lightquadVBO;

GLuint texture;

int width, height;
float rx = 0.0f, ry = 0.0f, pz = -70.0f;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * HVSSource
    = "#version 130\n in vec3 position; in vec2 tc; uniform mat4 mvp; out vec2 coord; void main() { gl_Position = mvp*vec4(position,1); coord = tc; }";
const char * HFSSource
    = "#version 130\n in vec2 coord; uniform sampler2D tex; out vec4 fragColor; void main() { fragColor = texture(tex, coord); }";

GLuint HVS, HFS, HProg;

GLuint HpositionAttrib, HtcAttrib, HmvpUniform;
GLuint HtextureUniform;

const char * SVSSource
    = "#version 130\n in vec3 position; in vec3 normal; uniform mat4 mvp; uniform vec3 lightvec; out float color; void main() { gl_Position = mvp*vec4(position,1); color = 0.3 + 0.7*max(0, dot(normal, lightvec)); }";
const char * SFSSource
    = "#version 130\n in float color; out vec4 fragColor; void main() { fragColor = vec4(color); }";

GLuint SVS, SFS, SProg;

GLuint SpositionAttrib, SnormalAttrib, SmvpUniform, SlightvecUniform;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Event handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void onInit()
{
    // Shaders
    HVS = compileShader(GL_VERTEX_SHADER, HVSSource);
    HFS = compileShader(GL_FRAGMENT_SHADER, HFSSource);
    HProg = linkShader(2, HVS, HFS);

    HpositionAttrib = glGetAttribLocation(HProg, "position");
    HtcAttrib = glGetAttribLocation(HProg, "tc");
    HmvpUniform = glGetUniformLocation(HProg, "mvp");

    HtextureUniform = glGetUniformLocation(HProg, "tex");

    SVS = compileShader(GL_VERTEX_SHADER, SVSSource);
    SFS = compileShader(GL_FRAGMENT_SHADER, SFSSource);
    SProg = linkShader(2, SVS, SFS);

    SpositionAttrib = glGetAttribLocation(SProg, "position");
    SnormalAttrib = glGetAttribLocation(SProg, "normal");
    SmvpUniform = glGetUniformLocation(SProg, "mvp");
    SlightvecUniform = glGetUniformLocation(SProg, "lightvec");

    // Copy house to graphics card
    glGenBuffers(1, &HouseVBO);
    glBindBuffer(GL_ARRAY_BUFFER, HouseVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(houseVertices), houseVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &HouseEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HouseEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(house), house, GL_STATIC_DRAW);

    // Copy sphere to graphics card
    glGenBuffers(1, &SphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, SphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &SphereEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere), sphere, GL_STATIC_DRAW);

    // Light quad
    glGenBuffers(1, &lightquadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightquadVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lightquad), lightquad, GL_STATIC_DRAW);

    //Load texture from file
    SDL_Surface * surface = SDL_LoadBMP("tiles.bmp");
    if(surface == NULL) throw SDL_Exception();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //ModelViewProjection
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

    //Draw house

    glUseProgram(HProg);

    glUniformMatrix4fv(HmvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

    // Texture 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(HtextureUniform, 0);

    glEnableVertexAttribArray(HpositionAttrib);
    glEnableVertexAttribArray(HtcAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, HouseVBO);
    glVertexAttribPointer(HpositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
    glVertexAttribPointer(HtcAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, texcoord));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HouseEBO);
    glDrawElements(GL_TRIANGLES, sizeof(house)/sizeof(*house), GL_UNSIGNED_BYTE, NULL);

    glDisableVertexAttribArray(HpositionAttrib);
    glDisableVertexAttribArray(HtcAttrib);

    //Draw "light"
    
    glUseProgram(SProg);
    glEnableVertexAttribArray(SpositionAttrib);

    glVertexAttrib3f(SnormalAttrib, 1, 0, 0);
    glUniform3f(SlightvecUniform, 1, 0, 0);
    glUniformMatrix4fv(SmvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindBuffer(GL_ARRAY_BUFFER, lightquadVBO);
    glVertexAttribPointer(SpositionAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Draw spheres

    glEnableVertexAttribArray(SnormalAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, SphereVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glVertexAttribPointer(SpositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, position));
    glVertexAttribPointer(SnormalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, normal));

    // Sphere 1
    glUniformMatrix4fv(SmvpUniform, 1, GL_FALSE, glm::value_ptr(glm::scale(glm::translate(mvp, glm::vec3(-2.1, -0.9, 0.9)), glm::vec3(1.8))));
    glUniform3fv(SlightvecUniform, 1, glm::value_ptr(glm::normalize(glm::vec3(0.0f, 5.0f, 0.0f) - glm::vec3(-2.1, -0.9, 0.9))));
    glDrawElements(GL_TRIANGLES, sizeof(sphere)/sizeof(**sphere), sphereIndexType, NULL);

    // Sphere 2
    glUniformMatrix4fv(SmvpUniform, 1, GL_FALSE, glm::value_ptr(glm::translate(mvp, glm::vec3(1.8, 2.3, -0.5))));
    glUniform3fv(SlightvecUniform, 1, glm::value_ptr(glm::normalize(glm::vec3(0.0f, 5.0f, 0.0f) - glm::vec3(1.8, 2.3, -0.5))));
    glDrawElements(GL_TRIANGLES, sizeof(sphere)/sizeof(**sphere), sphereIndexType, NULL);

    glDisableVertexAttribArray(SnormalAttrib);
    glDisableVertexAttribArray(SpositionAttrib);

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
