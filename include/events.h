#ifndef __BC23_EVENTS
#define __BC23_EVENTS

#include "ui.h"

void event_loop(void);

enum AppState {
    INIT,
    EDIT,
    PLAY
};

struct AppContext {
    enum AppState currentState;
    struct UIButton **buttons;
    size_t numButtons;
    SDL_MouseMotionEvent mouseMove;
    SDL_MouseButtonEvent mouseClick;
};

#endif