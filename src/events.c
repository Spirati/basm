#include "main.h"
#include "scenes.h"

#include <stdio.h>

void render(struct AppContext *context);

void event_loop(void) {
    int running = 1;
    // Initialize context wrapper
    struct AppContext context;
    context.shutdown = 0;
    context.scene_info = malloc(sizeof(union SceneInfo));
    context.scene = malloc(sizeof(struct UIScene));
    context.scene->buttons = malloc(sizeof(struct UIButton*)*MAX_BUTTONS);
    context.scene->texts = malloc(sizeof(struct UIText*)*MAX_MESSAGES);
    context.scene->numButtons = 0;
    context.scene->numTexts = 0;
    context.scene->freeButtonIndex = -1;
    context.scene->freeTextIndex = -1;

    // Set up initial scene
    event_switch_scene(&context, INIT, NULL);
    while(running) {
        if(context.shutdown) running = 0;
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
                case SDL_KEYDOWN:
                    switch(context.currentState) {
                        case EDIT:
                            _keyhandlerEDIT(&context, event.key);
                    }
                    break;
            }
        }
        if(running)
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

void event_switch_scene(struct AppContext *context, enum AppState state, union SceneInfo *pass) {
    for(size_t i = 0; i < context->scene->numButtons; i++) {
        delete_button(context, i);
    }
    for(size_t i = 0; i < context->scene->numTexts; i++) {
        delete_text(context, i);
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
        case APP_ERROR:
            free(context->scene_info->error);
            break;
    }
    free(context->scene_info);
    if(pass != NULL) {
        context->scene_info = pass;
    } else {
        context->scene_info = malloc(sizeof(union SceneInfo));
    }

    context->scene->numButtons = 0;
    context->scene->numTexts = 0;
    context->scene->freeButtonIndex = -1;
    context->scene->freeTextIndex = -1;

    switch(state) {
        case INIT:
            if(pass == NULL)
                context->scene_info->init = malloc(sizeof(struct InitSceneInfo));
            context->shutdown = _initINIT(context);
            break;
        case EDIT:
            if(pass == NULL)
                context->scene_info->edit = malloc(sizeof(struct EditSceneInfo));
            context->shutdown = _initEDIT(context);
            break;
        case PLAY:
            if(pass == NULL)
                context->scene_info->play = malloc(sizeof(struct PlaySceneInfo));
            context->shutdown = _initPLAY(context);
            break;
        case APP_ERROR:
            if(pass == NULL)
                context->scene_info->error = malloc(sizeof(struct ErrorSceneInfo));
            context->shutdown = _initERROR(context);
            break;
    }
}

int _initPLAY(struct AppContext *context) { return 0; }

void __test_button_callback(struct UIButtonCallbackInfo info) {
    SDL_SetWindowTitle(mainWindow, "THIS IS A TEST");
}

void __error_button_callback(struct UIButtonCallbackInfo info) {
    info.context->shutdown = 1;
}

union SceneInfo *event_generate_error(char *error) {
    union SceneInfo *pass = malloc(sizeof(union SceneInfo));
    struct ErrorSceneInfo *passError = malloc(sizeof(struct ErrorSceneInfo));
    passError->error = malloc(1024*sizeof(char));
    strncpy(passError->error, error, 1024);
    pass->error = passError;
    return pass;
}

void __error_button_file_select(struct UIButtonCallbackInfo info) {
    event_switch_scene(info.context, INIT, NULL);
}

int _initERROR(struct AppContext *context) {
    insert_text(context, TEXT_RED "An error has occured!", (SDL_Rect){ 0, 16, BASE_WIDTH, 0 }, CENTER);
    insert_text(context, context->scene_info->error->error, (SDL_Rect){ 32, 48, 0, 0 }, LEFT);

    insert_button(context, "Quit", (struct UIColor){ 255, 0, 0, 255 }, (struct UIColor){ 196, 0, 0, 255 }, 32, BASE_HEIGHT-64, 64, 32, &__error_button_callback);
    insert_button(context, "File select", (struct UIColor){ 255, 0, 0, 255 }, (struct UIColor) { 196, 0, 0, 255 }, 100, BASE_HEIGHT-64, 72, 32, &__error_button_file_select);

    context->currentState = APP_ERROR;

    return 0;
}