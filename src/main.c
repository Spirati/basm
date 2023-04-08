#include <stdio.h>
#include "main.h"

SDL_Window *mainWindow;
SDL_Renderer *mainRenderer;
struct TextRenderer *textRenderer;

int main(int argc, char **argv) {
    // Initialize SDL and window environment
    _init_SDL();
    SDL_CreateWindowAndRenderer(
        WIDTH, HEIGHT, 
        SDL_WINDOW_HIDDEN,
        &mainWindow, &mainRenderer
    );
    if(mainWindow == NULL || mainRenderer == NULL)
        _quit_SDL(1);
    
    SDL_SetWindowPosition(mainWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowTitle(mainWindow, "Bitcamp 2023");
    SDL_RenderSetLogicalSize(mainRenderer, BASE_WIDTH, BASE_HEIGHT);
    SDL_RenderPresent(mainRenderer);
    SDL_ShowWindow(mainWindow);

    textRenderer = text_create_renderer(mainRenderer);

    event_loop();

    _quit_SDL(0);
    return 0;
}

void _init_SDL(void) {
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0) {
        _quit_SDL(1);
    }
}
void _quit_SDL(int error) {
    if(error != 0) {
        printf("SDL error: %s\n", SDL_GetError());
    }
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    text_destroy_renderer(textRenderer);
    SDL_Quit();
}
