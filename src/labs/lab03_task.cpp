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

const glm::vec3 positions[4] = { glm::vec3(-2, 0, -2), glm::vec3(-2, 0, 2), glm::vec3(2, 0, 2), glm::vec3(2, 0, -2) };
const float colors[4][3] = { { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }, { 0, 1, 1 } };

struct Vertex {
    float position[3];
    float normal[3];
} const scene[] = {
    // floor
    { { -10, 0,  10 }, { 0, 1, 0 } },
    { {  10, 0,  10 }, { 0, 1, 0 } },
    { {  10, 0, -10 }, { 0, 1, 0 } },
    { { -10, 0, -10 }, { 0, 1, 0 } },
    // box top
    { { -1, 2,  1 }, { 0, 1, 0 } },
    { {  1, 2,  1 }, { 0, 1, 0 } },
    { {  1, 2, -1 }, { 0, 1, 0 } },
    { { -1, 2, -1 }, { 0, 1, 0 } },
    // box front
    { { -1, 0, 1 }, { 0, 0, 1 } },
    { {  1, 0, 1 }, { 0, 0, 1 } },
    { {  1, 2, 1 }, { 0, 0, 1 } },
    { { -1, 2, 1 }, { 0, 0, 1 } },
    // box left
    { { -1, 0, -1 }, { -1, 0, 0 } },
    { { -1, 0,  1 }, { -1, 0, 0 } },
    { { -1, 2,  1 }, { -1, 0, 0 } },
    { { -1, 2, -1 }, { -1, 0, 0 } },
    // box back
    { {  1, 0, -1 }, { 0, 0, -1 } },
    { { -1, 0, -1 }, { 0, 0, -1 } },
    { { -1, 2, -1 }, { 0, 0, -1 } },
    { {  1, 2, -1 }, { 0, 0, -1 } },
    // box right
    { { 1, 0,  1 }, { 1, 0, 0 } },
    { { 1, 0, -1 }, { 1, 0, 0 } },
    { { 1, 2, -1 }, { 1, 0, 0 } },
    { { 1, 2,  1 }, { 1, 0, 0 } }
};

const unsigned char box[] = {
    0, 1, 2,
    0, 2, 3,
    4, 5, 6,
    4, 6, 7,
    8, 9, 10,
    8, 10, 11,
    12, 13, 14,
    12, 14, 15,
    16, 17, 18,
    16, 18, 19
};

const glm::vec4 shadow_quad[4] = {
    glm::vec4(-1, -1.5,  1, 1),
    glm::vec4( 1, -1.5,  1, 1),
    glm::vec4( 1, -1.5, -1, 1),
    glm::vec4(-1, -1.5, -1, 1)
};

GLuint sceneVBO, sceneEBO;
GLuint sphereVBO, sphereEBO;
GLuint VBO;

int width, height;
float rx = 0.0f, ry = 0.0f, pz = -50.0f;

bool draw_volume = false;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Shaders
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const char * VSSource
    = "#version 130\n in vec4 position; in vec3 normal; uniform mat4 mvp; uniform vec3 trans; out vec3 p, n; void main() { gl_Position = mvp*position; p = position.xyz+trans; n = normal; }";
const char * FSSource
    = "#version 130\n in vec3 p, n; out vec4 fragColor; uniform vec3 color, la, ld, lightPos; void main() { fragColor = vec4(color*la + color*ld*max(0.0, dot(normalize(n), normalize(lightPos-p))), 1); }";

GLuint VS, FS, Prog;

GLuint positionAttrib, normalAttrib, mvpUniform, transUniform, colorUniform;
GLuint laUniform, ldUniform, lightPosUniform;

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
    normalAttrib = glGetAttribLocation(Prog, "normal");
    mvpUniform = glGetUniformLocation(Prog, "mvp");
    transUniform = glGetUniformLocation(Prog, "trans");
    colorUniform = glGetUniformLocation(Prog, "color");
    laUniform = glGetUniformLocation(Prog, "la");
    ldUniform = glGetUniformLocation(Prog, "ld");
    lightPosUniform = glGetUniformLocation(Prog, "lightPos");

    //Copy scene to graphics card
    glGenBuffers(1, &sceneVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sceneVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(scene), scene, GL_STATIC_DRAW);

    glGenBuffers(1, &sceneEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sceneEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);

    //Useful buffer
    glGenBuffers(1, &VBO);

    //Sphere
    glGenBuffers(1, &sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &sphereEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere), sphere, GL_STATIC_DRAW);
}

void drawScene(const glm::mat4 & matrix)
{
    glm::mat4 mvp = glm::translate(matrix, glm::vec3(0, -5, 0));
    glBindBuffer(GL_ARRAY_BUFFER, sceneVBO);
    //Draw floor
    glUniform3f(colorUniform, 0, 1, 0);
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3f(transUniform, 0, -5, 0);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    //Draw four boxes
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex)*4 + offsetof(Vertex, position)));
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex)*4 + offsetof(Vertex, normal)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sceneEBO);
    for(unsigned i = 0; i < 4; ++i)
    {
        glUniform3fv(colorUniform, 1, colors[i]);
        glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(glm::translate(mvp, positions[i])));
        glUniform3fv(transUniform, 1, glm::value_ptr(positions[i] + glm::vec3(0, -5, 0)));
        glDrawElements(GL_TRIANGLES, sizeof(box)/sizeof(*box), GL_UNSIGNED_BYTE, NULL);
    }
}

void drawShadowQuad()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //allocate buffer memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*4, NULL, GL_STREAM_DRAW);
    //fill buffer
    float *buffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(buffer     , glm::value_ptr(shadow_quad[0]), sizeof(float)*4);
        memcpy(buffer + 4 , glm::value_ptr(shadow_quad[1]), sizeof(float)*4);
        memcpy(buffer + 8 , glm::value_ptr(shadow_quad[2]), sizeof(float)*4);
        memcpy(buffer + 12, glm::value_ptr(shadow_quad[3]), sizeof(float)*4);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    //draw contents
    glDisableVertexAttribArray(normalAttrib);
    glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);//draw shading quad
    glEnableVertexAttribArray(normalAttrib);
}

void drawShadowVolume()
{
    glm::vec4 light_pos(0, 0, 0, 1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //allocate buffer memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*10, NULL, GL_STREAM_DRAW);
    //fill buffer
    float *buffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(buffer     , glm::value_ptr(shadow_quad[0]), sizeof(float)*4);
        memcpy(buffer + 4 , glm::value_ptr(shadow_quad[0] - light_pos), sizeof(float)*4);
        memcpy(buffer + 8 , glm::value_ptr(shadow_quad[1]), sizeof(float)*4);
        memcpy(buffer + 12, glm::value_ptr(shadow_quad[1] - light_pos), sizeof(float)*4);
        memcpy(buffer + 16, glm::value_ptr(shadow_quad[2]), sizeof(float)*4);
        memcpy(buffer + 20, glm::value_ptr(shadow_quad[2] - light_pos), sizeof(float)*4);
        memcpy(buffer + 24, glm::value_ptr(shadow_quad[3]), sizeof(float)*4);
        memcpy(buffer + 28, glm::value_ptr(shadow_quad[3] - light_pos), sizeof(float)*4);
        memcpy(buffer + 32, glm::value_ptr(shadow_quad[0]), sizeof(float)*4);//first two again
        memcpy(buffer + 36, glm::value_ptr(shadow_quad[0] - light_pos), sizeof(float)*4);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    //draw contents
    glDisableVertexAttribArray(normalAttrib);
    glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(float)*8, NULL);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);//draw shading quad
    glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);//draw sides
    glEnableVertexAttribArray(normalAttrib);
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // View-Projection matrix
    glm::mat4 vp = 
            glm::rotate(
                glm::rotate(
                    glm::translate(
                        glm::perspective(45.0f, (float)width/(float)height, 1.0f, 1000.0f),
                        glm::vec3(0, 0, pz)
                        ),
                    ry, glm::vec3(1, 0, 0)
                    ),
                rx, glm::vec3(0, 1, 0)
                );

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glUseProgram(Prog);

    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(normalAttrib);

    //Render depth and ambient color
    glUniform3f(laUniform, 0.2, 0.2, 0.2);
    glUniform3f(ldUniform, 0, 0, 0);
    drawScene(vp);

    glDepthMask(GL_FALSE);

    //Set shadow mask for light
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(vp));
    drawShadowVolume();

    //Add lit parts
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glStencilFunc(GL_EQUAL, 0, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glUniform3f(laUniform, 0, 0, 0);
    glUniform3f(ldUniform, 0.5, 0.5, 0.5);
    glUniform3f(lightPosUniform, 0, 0, 0);
    drawScene(vp);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);

    glDepthMask(GL_TRUE);

    //Draw shadow quad
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(vp));
    glUniform3f(colorUniform, 1, 0, 0);
    glUniform3f(laUniform, 1, 1, 1);
    glUniform3f(ldUniform, 0, 0, 0);
    drawShadowQuad();

    //Draw shadow volume
    if(draw_volume)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawShadowVolume();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Draw light
    glUniform3f(colorUniform, 1, 1, 0);
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(glm::scale(vp, glm::vec3(0.5))));
    glDisableVertexAttribArray(normalAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, position));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glDrawElements(GL_TRIANGLES, sizeof(sphere)/sizeof(*sphere)*3, sphereIndexType, NULL);

    SDL_GL_SwapBuffers();
}

void onWindowResized(int w, int h)
{
    width = w; height = h;
    glViewport(0, 0, w, h);
}

void onKeyDown(SDLKey key, Uint16 /*mod*/)
{
    switch(key) {
        case SDLK_ESCAPE : quit(); break;
        case SDLK_v : draw_volume = !draw_volume; break;
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

        init(800, 600, 24, 24, 8);

        mainLoop(40);//40ms == 25fps

    } catch(exception & ex) {
        cout << "ERROR : " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
