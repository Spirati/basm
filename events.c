#include "main.h"

void event_loop(void) {
    int running = 1;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = 0;
                default:
                    continue;
            }
        }
    }
}