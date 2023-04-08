#include "main.h"

void switch_scene(struct AppContext *context, enum AppState state);
void render(struct AppContext *context);

void _initINIT(struct AppContext *context);
void _initEDIT(struct AppContext *context);
void _initPLAY(struct AppContext *context);

void __test_button_callback(void);
void ui_dummy_callback(void) {}


void event_loop(void) {
    int running = 1;
    // Initialize context wrapper
    struct AppContext context;
    context.buttons = malloc(sizeof(struct UIButton*)*20);
    context.numButtons = 0;

    // Set up initial scene
    switch_scene(&context, INIT);
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEMOTION:
                    context.mouseMove = event.motion;
                    break;
                case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
                    context.mouseClick = event.button;
            }
        }
        render(&context);
    }
}

void render(struct AppContext *context) {
    SDL_RenderClear(mainRenderer);

    SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);
    SDL_RenderFillRect(mainRenderer, &(SDL_Rect){ 0, 0, BASE_WIDTH, BASE_HEIGHT });
    // Render buttons on screen (TODO: extend to more UI elements)
    for(size_t i = 0; i < context->numButtons; i++) {
        struct UIButton *button = context->buttons[i];
        ui_draw_button(button, context);
    }

    SDL_RenderPresent(mainRenderer);
}

void switch_scene(struct AppContext *context, enum AppState state) {
    for(size_t i = 0; i < context->numButtons; i++) free(context->buttons[i]);

    switch(state) {
        case INIT:
            _initINIT(context);
            break;
        case EDIT:
            _initEDIT(context);
            break;
        case PLAY:
            _initPLAY(context);
            break;
    }
}

// Insert a button into the scene and increment the button counter
void insert_button(struct AppContext *context, char *text, struct UIColor neutralColor, struct UIColor hoverColor, int x, int y, int w, int h, void (*on_click)(void)) {
    context->buttons[context->numButtons++] = malloc(sizeof(struct UIButton));
    ui_create_button(
        context->buttons[context->numButtons-1],
        text,
        neutralColor,
        hoverColor,
        x,y,w,h,
        on_click
    );
}

void _initINIT(struct AppContext *context) {
    context->numButtons = 0;

    insert_button(
        context,
        "Hello!", 
        (struct UIColor){255,0,0,255}, (struct UIColor){128,0,0,255}, 
        100,100,64,16, 
        &__test_button_callback
    );
    insert_button(
        context,
        "Button 2", 
        (struct UIColor){0,255,0,255}, (struct UIColor){0,128,0,255},
        24,24,48,48,
        &ui_dummy_callback
    );

    context->currentState = INIT;
}

void _initEDIT(struct AppContext *context) {}
void _initPLAY(struct AppContext *context) {}

void __test_button_callback(void) {
    SDL_SetWindowTitle(mainWindow, "THIS IS A TEST");
}