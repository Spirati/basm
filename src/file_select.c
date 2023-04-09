#include "events.h"
#include "scenes.h"
#include "def.h"
#include "text.h"
#include <stdio.h>
#include <windows.h>
#include <string.h>

void ui_dummy_callback(struct UIButtonCallbackInfo info) {}

void scenes_init_open_file(struct AppContext *context, char *filename) {
    union SceneInfo *pass = malloc(sizeof(union SceneInfo));
    struct PlaySceneInfo *passPlay = malloc(sizeof(struct PlaySceneInfo));
    passPlay->filename = malloc(1024*sizeof(char));
    strncpy(passPlay->filename, filename, 1024);
    pass->play = passPlay;
    event_switch_scene(context, PLAY, pass);
}

void scenes_init_load_file_callback(struct UIButtonCallbackInfo info) {
    scenes_init_open_file(info.context, info.button->text);
}

void scenes_init_list_programs(struct UIButtonCallbackInfo info) {
    // delete old buttons
    if(info.context->scene->numButtons > info.context->scene_info->init->toolbar_end) {
        for(size_t filenameIndex = info.context->scene->numButtons-1; filenameIndex >= info.context->scene_info->init->toolbar_end; filenameIndex--) {
            delete_button(info.context, filenameIndex);
            info.context->scene->numButtons--; // in THIS PARTICULAR CASE we can do this
        }
    }
    if(info.context->scene_info->init->warning_message_index >= 0) {
        delete_text(info.context, info.context->scene_info->init->warning_message_index);
        info.context->scene_info->init->warning_message_index = -1;
    }

    WIN32_FIND_DATA find;
    HANDLE hFind = NULL;
    if((hFind = FindFirstFile("programs/*.basm", &find)) == INVALID_HANDLE_VALUE) {
        printf("no files\n");
        info.context->scene_info->init->warning_message_index = insert_text(info.context, "No programs found", (SDL_Rect){ 0, 48, BASE_WIDTH, 0 }, CENTER);
        FindClose(hFind);
        return;
    }

    #define ROW_WIDTH 3
    struct UIButton *buttonBuffer = malloc(ROW_WIDTH*sizeof(struct UIButton));
    char **stringBuffer = malloc(ROW_WIDTH*sizeof(char *));
    for(size_t i = 0; i < ROW_WIDTH; i++) {
        stringBuffer[i] = malloc(1024*sizeof(char));
    }
    int row = 0;
    size_t buttonIndex = 0;
    do {
        strcpy(stringBuffer[buttonIndex], find.cFileName);
        buttonBuffer[buttonIndex] = (struct UIButton){
            stringBuffer[buttonIndex],
            (struct UIColor){ 0, 0, 0, 255 },
            (struct UIColor){ 32, 32, 32, 255 },
            0,0,0,0,
            &scenes_init_load_file_callback
        };
        buttonIndex++;
        if(buttonIndex == ROW_WIDTH) { // flush button buffer and create row
            insert_button_row(
                info.context, ROW_WIDTH, buttonBuffer, (SDL_Rect){ 0, 48+16*(row++), BASE_WIDTH, 9 }, 0
            );
            buttonIndex = 0;
        }
    } while(FindNextFile(hFind, &find));
    if(buttonIndex > 0) { // remaining to be flushed
        for(size_t i = buttonIndex; i < ROW_WIDTH; i++) {
            strcpy(stringBuffer[i], "");
            buttonBuffer[i] = (struct UIButton){
                "",
                (struct UIColor){ 0, 0, 0, 255 },
                (struct UIColor){ 0, 0, 0, 255 },
                0,0,0,0,
                &ui_dummy_callback
            };
        }
        insert_button_row(
            info.context, ROW_WIDTH, buttonBuffer, (SDL_Rect){ 0, 48+16*(row++), BASE_WIDTH, 16 }, 0
        );
    }
    for(size_t i = 0; i < ROW_WIDTH; i++) {
        free(stringBuffer[i]);
    }
    free(buttonBuffer);
    FindClose(hFind);
}

void error_button(struct UIButtonCallbackInfo info) {
    event_switch_scene(info.context, APP_ERROR, event_generate_error("This is an example of error text."));
}


int _initINIT(struct AppContext *context) {
    insert_button_row(
        context,
        3,
        (struct UIButton[]){
            (struct UIButton){ 
                "error moment",
                (struct UIColor){ 255, 0, 0, 255 },
                (struct UIColor){ 128, 0, 0, 255 },
                0,0,0,0,
                &error_button
            },
            (struct UIButton){ 
                "Refresh",
                (struct UIColor){ 0, 255, 0, 255 },
                (struct UIColor){ 0, 128, 0, 255 },
                0,0,0,0,
                &scenes_init_list_programs
            },
            (struct UIButton){ 
                "Button 3",
                (struct UIColor){ 255, 0, 0, 255 },
                (struct UIColor){ 128, 0, 0, 255 },
                0,0,0,0,
                &ui_dummy_callback
            }
        },
        (SDL_Rect){ 0, 0, BASE_WIDTH, 32 },
        1
    );
    context->scene_info->init->toolbar_end = context->scene->numButtons;
    context->scene_info->init->warning_message_index = -1;

    scenes_init_list_programs((struct UIButtonCallbackInfo) {
        NULL, context
    });

    context->currentState = INIT;

    return 0;
}
