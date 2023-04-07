#include <stdio.h>
#include "main.h"

int main(int argc, char **argv) {
    init_SDL();
    SDL_Window *mainWindow;
    SDL_Renderer *mainRenderer;
    SDL_CreateWindowAndRenderer(
        WIDTH, HEIGHT, 
        SDL_WINDOW_HIDDEN,
        &mainWindow, &mainRenderer
    );
    if(mainWindow == NULL || mainRenderer == NULL)
        quit_SDL(1);
    
    SDL_SetWindowPosition(mainWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowTitle(mainWindow, "Bitcamp 2023");
    SDL_RenderSetLogicalSize(mainRenderer, BASE_WIDTH, BASE_HEIGHT);
    SDL_RenderPresent(mainRenderer);
    SDL_ShowWindow(mainWindow);

    event_loop();

    quit_SDL(0);
    return 0;
}

void init_SDL(void) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        quit_SDL(1);
    }
}
void quit_SDL(int error) {
    if(error != 0) {
        printf("SDL error: %s\n", SDL_GetError());
    }
    SDL_Quit();
}
