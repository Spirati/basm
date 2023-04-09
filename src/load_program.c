#include "events.h"
#include "scenes.h"
#include "def.h"
#include "text.h"
#include "basm/basm.h"
#include <stdio.h>
#include <string.h>

void __file_select(struct UIButtonCallbackInfo info) {
    event_switch_scene(info.context, INIT, NULL);
}

void _keyhandlerEDIT(struct AppContext *context, SDL_KeyboardEvent event) {
    if(event.keysym.sym == SDLK_DOWN && context->scene_info->edit->linecount > 16) {
        
        int scrollStart = context->scene_info->edit->scrollStart;
        char *fulltext = context->scene_info->edit->fulltext;
        int scrollEnd = context->scene_info->edit->scrollEnd;
        if(scrollEnd == context->scene_info->edit->filelength-1) return;
        int search;
        int start; int stop;
        for(search = scrollStart; search < scrollEnd; search++) {
            if(search != scrollStart && fulltext[search] == '\n') {
                start = search+1;
                context->scene_info->edit->scrollStart = start;
                break;
            }
        }
        for(search = scrollEnd; search < context->scene_info->edit->filelength; search++) {
            if(search != scrollEnd && fulltext[search] == '\n') {
                stop = search;
                context->scene_info->edit->scrollEnd = search;
                break;
            }
        }
        memset(context->scene_info->edit->showntext, 0, 4096*sizeof(char));
        strncpy(context->scene_info->edit->showntext, fulltext+start, stop-start);
    }
    if(event.keysym.sym == SDLK_UP && context->scene_info->edit->linecount > 16) {
        int scrollStart = context->scene_info->edit->scrollStart;
        char *fulltext = context->scene_info->edit->fulltext;
        int scrollEnd = context->scene_info->edit->scrollEnd;
        if(scrollStart <= 2) return;
        int search;
        int start; int stop;
        for(search = scrollStart-2; search >= 0; search--) {
            if(fulltext[search] == '\n' || search == 0) {
                start = search;
                context->scene_info->edit->scrollStart = start;
                break;
            }
        }
        for(search = scrollEnd; search > start; search--) {
            if(search != scrollEnd && fulltext[search] == '\n') {
                stop = search;
                context->scene_info->edit->scrollEnd = search;
                break;
            }
        }
        memset(context->scene_info->edit->showntext, 0, 4096*sizeof(char));
        strncpy(context->scene_info->edit->showntext, fulltext+start, stop-start);
    }
}

int _initEDIT(struct AppContext *context) {

    context->scene_info->edit->fulltext = malloc(4096*sizeof(char));

    char to_open[2048];
    char *filename = context->scene_info->edit->filename;
    sprintf(to_open, "programs/%s", filename);

    char buffer[4096];
    memset(buffer, 0, 4096);
    FILE *f = fopen(to_open, "r");
    size_t len = fread(buffer, sizeof(char), 4096, f);
    fclose(f);

    if(buffer[4095] != '\0') {
        fprintf(stderr, "Input file too long\n");
        return 1;
    }

    char header[2048];
    sprintf(header, TEXT_PINK "%s" TEXT_WHITE " (%zd bytes, max 4096)", filename, len);

    insert_text(context, header, (SDL_Rect){ 0, 16, BASE_WIDTH, 0 }, CENTER);

    struct BASMParseOutput parse_output = basm_parse(buffer);
    if(parse_output.error != NULL) {
        union SceneInfo *pass = event_generate_error(parse_output.error);
        event_switch_scene(context, APP_ERROR, pass);
        return 0;
    }
    context->scene_info->edit->fulltext = parse_output.syntax_highlighted;
    char *fulltext = context->scene_info->edit->fulltext;

    context->scene_info->edit->filelength = strlen(fulltext);

    context->scene_info->edit->scrollStart = 0;
    size_t search;
    size_t linecount = 0;
    for(search = 0; fulltext[search] != '\0'; search++) {
        if(fulltext[search] == '\n') {
            if(++linecount == 16) break;
        }
    }
    if(search != context->scene_info->edit->filelength) linecount++;
    context->scene_info->edit->linecount = linecount; // really just a >16 check
    context->scene_info->edit->scrollEnd = search;

    context->scene_info->edit->showntext = malloc(4096*sizeof(char));
    char *showntext = context->scene_info->edit->showntext;


    strncpy(showntext, context->scene_info->edit->fulltext+context->scene_info->edit->scrollStart, context->scene_info->edit->scrollEnd);
    
    size_t text_index = insert_text(context, context->scene_info->edit->showntext, (SDL_Rect){ 16, 40, 0, 0 }, LEFT);
    free(context->scene->texts[text_index]->text);
    context->scene->texts[text_index]->text = showntext;

    // insert_text(context, TEXT_DARK_GREY "(You can scroll with \nthe arrow keys)", (SDL_Rect){16, BASE_HEIGHT-32, 0, 0 }, LEFT);

    insert_button(context, "File select", (struct UIColor){ 255, 0, 0, 255 }, (struct UIColor) { 196, 0, 0, 255 }, BASE_WIDTH-16-72, BASE_HEIGHT-16-32, 72, 32, &__file_select);
    context->currentState = EDIT;

    struct BASMAssembleOutput assembled = basm_assemble(parse_output.token_stream, parse_output.token_count);
    if(assembled.error != NULL) {
        union SceneInfo *pass = event_generate_error(assembled.error);
        event_switch_scene(context, APP_ERROR, pass);
    }

    return 0;

}