#ifndef __BC23_MAIN
#define __BC23_MAIN

#include "SDL/SDL.h"
#include "SDL_image/SDL_image.h"
#include "events.h"

#define ASPECT_RATIO    (256.0/224.0)
#define SCALE           2
#define BASE_HEIGHT     224
#define HEIGHT          (BASE_HEIGHT*SCALE)
#define BASE_WIDTH      ((int)((float)BASE_HEIGHT*ASPECT_RATIO))
#define WIDTH           (BASE_WIDTH*SCALE)      

void _init_SDL(void);
void _quit_SDL(int error);

extern SDL_Window *mainWindow;
extern SDL_Renderer *mainRenderer;

#endif