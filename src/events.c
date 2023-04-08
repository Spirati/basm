#include "main.h"
#include "scenes.h"

#include <stdio.h>

void switch_scene(struct AppContext *context, enum AppState state);
void render(struct AppContext *context);

void event_loop(void) {
    int running = 1;
    // Initialize context wrapper
    struct AppContext context;
    context.scene_info = malloc(sizeof(union SceneInfo));
    context.scene = malloc(sizeof(struct UIScene));
    context.scene->buttons = malloc(sizeof(struct UIButton*)*MAX_BUTTONS);
    context.scene->texts = malloc(sizeof(struct UIText*)*MAX_MESSAGES);
    context.scene->numButtons = 0;
    context.scene->numTexts = 0;
    context.scene->freeButtonIndex = -1;
    context.scene->freeTextIndex = -1;

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
    for(size_t i = 0; i < context->scene->numButtons + (context->scene->freeButtonIndex >= 0 ? 1 : 0); i++) {
        struct UIButton *button = context->scene->buttons[i];
        if(button == NULL) continue;
        ui_draw_button(button, context);
    }
    for(size_t i = 0; i < context->scene->numTexts + (context->scene->freeTextIndex >= 0 ? 1 : 0); i++) {
        struct UIText *text = context->scene->texts[i];
        if(text == NULL) continue;
        text_write(textRenderer, text->text, mainRenderer, &text->anchor, text->align);
    }

    SDL_RenderPresent(mainRenderer);
}

void switch_scene(struct AppContext *context, enum AppState state) {
    for(size_t i = 0; i < context->scene->numButtons; i++) {
        free(context->scene->buttons[i]->text);
        free(context->scene->buttons[i]);
    }
    for(size_t i = 0; i < context->scene->numTexts; i++) {
        free(context->scene->texts[i]->text);
        free(context->scene->texts[i]);
    }
    switch(context->currentState) {
        case INIT:
            free(context->scene_info->init);
            break;
        case EDIT:
            free(context->scene_info->edit);
            break;
        case PLAY:
            free(context->scene_info->play);
            break;
    }
    free(context->scene_info);
    context->scene_info = malloc(sizeof(union SceneInfo));

    context->scene->numButtons = 0;
    context->scene->numTexts = 0;
    context->scene->freeButtonIndex = -1;
    context->scene->freeTextIndex = -1;

    switch(state) {
        case INIT:
            context->scene_info->init = malloc(sizeof(struct InitSceneInfo));
            _initINIT(context);
            break;
        case EDIT:
            context->scene_info->edit = malloc(sizeof(struct EditSceneInfo));
            _initEDIT(context);
            break;
        case PLAY:
            context->scene_info->play = malloc(sizeof(struct PlaySceneInfo));
            _initPLAY(context);
            break;
    }
}


void _initEDIT(struct AppContext *context) {}
void _initPLAY(struct AppContext *context) {}

void __test_button_callback(struct UIButtonCallbackInfo info) {
    SDL_SetWindowTitle(mainWindow, "THIS IS A TEST");
}