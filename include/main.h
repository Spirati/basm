#ifndef __BC23_MAIN
#define __BC23_MAIN

#include "SDL/SDL.h"
#include "SDL_image/SDL_image.h"
#include "events.h"
#include "text.h"
#include "def.h"      

void _init_SDL(void);
void _quit_SDL(int error);

extern SDL_Window *mainWindow;
extern SDL_Renderer *mainRenderer;
extern struct TextRenderer *textRenderer;

#endif