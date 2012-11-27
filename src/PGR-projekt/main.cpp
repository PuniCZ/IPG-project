
#include <SDL/SDL.h>
#include <string>
#include <exception>
#include <stdexcept>
#include <iostream>

#include "CameraPlane.h"
#include "Raytracer.h"




void PutPixel32_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32*)pixel) = color;
}


void PutPixel32(SDL_Surface * surface, int x, int y, Uint32 color)
{
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
    PutPixel32_nolock(surface, x, y, color);
    if( SDL_MUSTLOCK(surface) )
        SDL_UnlockSurface(surface);
}



int main (int /*argc*/, char ** /*argv*/)
{
    // Init SDL - only video subsystem will be used
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
        return EXIT_FAILURE;

    // Shutdown SDL when program ends
    atexit(SDL_Quit);

    
    SDL_WM_SetCaption("Raytracing clouds", NULL);

    SDL_Surface *screen = SDL_SetVideoMode( 800 , 600 , 32 , SDL_HWSURFACE |SDL_ANYFORMAT); // | SDL_DOUBLEBUF

    CameraPlane camera = CameraPlane(glm::vec3(0, 0 , -5), glm::vec3(0, 0, 0), glm::vec2(screen->w, screen->h));

    ScreenBuffer* buffer = camera.GetBuffer();

    Scene scene = Scene();

    scene.Init();

    Raytracer raytracer = Raytracer(&camera);
    
    //raytracer.Render(scene);

    SDL_Event event;
    bool done = false;
    bool renderFinished = false;
    while(!done) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
            done=true;
            }
        }

        if (!renderFinished)
        {    renderFinished = raytracer.Render(scene);

        for (int i = 0; i < screen->w ; i++)
        {
            for (int j = raytracer.getCurrentLine()-1; j < raytracer.getCurrentLine() ; j++)
            {
                glm::vec3 color = buffer->GetPixel(i, j);
                PutPixel32(screen, i, j, SDL_MapRGBA(screen->format, color.r*255.f, color.g*255.f, color.b*255.f, 255));
            }
        }
        // update the screen buffer	
		//SDL_Flip(screen);
		SDL_UpdateRect(screen, 0, 0,  screen->w,  screen->h);
		}
		
    }


    return EXIT_SUCCESS;
}