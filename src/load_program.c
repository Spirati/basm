#include "events.h"
#include "scenes.h"
#include "def.h"
#include "text.h"
#include "basm/basm.h"
#include <stdio.h>
#include <string.h>

int _initPLAY(struct AppContext *context) {
    char to_open[2048];
    char *filename = context->scene_info->play->filename;
    sprintf(to_open, "programs/%s", filename);

    char contents_buffer[4096];
    memset(contents_buffer, 0, 4096);
    FILE *f = fopen(to_open, "r");
    size_t len = fread(contents_buffer, sizeof(char), 4096, f);
    fclose(f);

    if(contents_buffer[4095] != '\0') {
        fprintf(stderr, "Input file too long\n");
        return 1;
    }

    char header[2048];
    sprintf(header, TEXT_PINK "%s" TEXT_WHITE " (%zd/4096 bytes)", filename, len);

    insert_text(context, header, (SDL_Rect){ 0, 16, BASE_WIDTH, 0 }, CENTER);

    struct BASMParseOutput parse_output = basm_parse(contents_buffer);
    if(parse_output.error != NULL) {
        union SceneInfo *pass = event_generate_error(parse_output.error);
        event_switch_scene(context, APP_ERROR, pass);
        return 0;
    }
    
    insert_text(context, parse_output.syntax_highlighted, (SDL_Rect){ 16, 48, 0, 0 }, LEFT);
    context->currentState = PLAY;

    return 0;

}