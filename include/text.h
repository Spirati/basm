#ifndef __BC23_TEXT
#define __BC23_TEXT

#include "main.h"

struct TextRenderer {
    SDL_Texture *font;
};

enum TextAlign {
    LEFT, CENTER, RIGHT,
};

extern struct TextRenderer *textRenderer;

struct TextRenderer *text_create_renderer(SDL_Renderer *renderer);

void text_write(struct TextRenderer *textRenderer, char *text, SDL_Renderer *renderer, const SDL_Rect *anchor, enum TextAlign align);

void text_destroy_renderer(struct TextRenderer *textRenderer);

#endif