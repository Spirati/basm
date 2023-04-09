#include "main.h"
#include <stdio.h>

struct TextRenderer *text_create_renderer(SDL_Renderer *renderer) {
    struct TextRenderer *textRenderer = malloc(sizeof(struct TextRenderer));
    textRenderer->font = IMG_LoadTexture(renderer, "resource/font_wide.png");

    return textRenderer;
}

void text_destroy_renderer(struct TextRenderer *renderer) {
    SDL_DestroyTexture(renderer->font);
}

// account for stupid hardcoding
int _calculate_char_offset(char c) {


    if(c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    if(c >= 'a' && c <= 'z') {
        return c - 'a' + ('Z' - 'A') + 1;
    }
    if(c > '0' && c <= '9') {
        return c - '1' + ('Z' - 'A') + ('z' - 'a') + 2;
    }
    if(c == '0') {
        return _calculate_char_offset('9')+1;
    }
    int O = _calculate_char_offset('0');
    switch(c) {
        case '(':
            return O + 1;
        case ')':
            return O + 2;
        case '[':
            return O + 3;
        case ']':
            return O + 4;
        case '{':
            return O + 5;
        case '}':
            return O + 6;
        case '<':
            return O + 7;
        case '>':
            return O + 8;
        case ' ':
            return O + 9;
        case '!':
            return O + 10;
        case '+':
            return O + 11;
        case '-':
            return O + 12;
        case '*':
            return O + 13;
        case '=':
            return O + 14;
        case '?':
            return O + 15;
        case '/':
            return O + 16;
        case '\\':
            return O + 17;
        case '.':
            return O + 18;
        case ',':
            return O + 20;
        case ';':
            return O + 21;
        case ':':
            return O + 22;
        case '"':
            return O + 23;
        case '\'':
            return O + 24;
        case '~':
            return O + 25;
        case '`':
            return O + 26;
        case '^':
            return O + 27;
        case '_':
            return O + 28;
        case '|':
            return O + 29;
        case '@':
            return O + 30;
        case '#':
            return O + 31;
        case '$':
            return O + 32;
        case '%':
            return O + 33;
        case '&':
            return O + 34;
    }
    return _calculate_char_offset(' ');
}

// only LEFT supports line breaks for now
void text_write(struct TextRenderer *textRenderer, char *text, SDL_Renderer *renderer, const SDL_Rect *anchor, enum TextAlign align) {
    size_t c;
    size_t line = 0;
    size_t col = 0;
    SDL_SetTextureColorMod(textRenderer->font, 255, 255, 255);
    switch(align) {
        case LEFT:
            // for(c = 0; text[c] != '\0'; c++) {
            //     if(c == '\xff') { // start of color block
            //         char *color = text+c+1; // pretend it's an array
            //         SDL_SetTextureColorMod(textRenderer->font, color[0], color[1], color[2]);
            //         c += 3;
            //     } else {
            //         if(text[c] == '\n') {
            //             col = 0;
            //             line++;
            //         } else {
            //             SDL_RenderCopy(renderer, textRenderer->font, &(SDL_Rect){ 5*_calculate_char_offset(text[c]), 0, 5, 9 }, &(SDL_Rect){ anchor->x+5*(col++), anchor->y+9*line, 5, 9 });
            //         }
            //     }
                
            // }
            // break;
            col = 0;
            line = 0;
            for(c = 0; text[c] != '\0'; c++) {
                if(text[c] == '\xff') {
                    // SDL_SetTextureColorMod(textRenderer->font, 255, 255, 255);
                    char *color = text+c+1;
                    SDL_SetTextureColorMod(textRenderer->font, color[0], color[1], color[2]);
                    c += 3;
                } else {
                    if(text[c] == '\n') {
                        col = 0;
                        line++;
                        continue;
                    }
                    SDL_RenderCopy(renderer, textRenderer->font, &(SDL_Rect){ 5*_calculate_char_offset(text[c]), 0, 5, 9 }, &(SDL_Rect){ anchor->x+5*(col++), anchor->y+10*line, 5, 9 });
                }
            }
            break;
        case RIGHT:
            c = 0;
            while(text[c++] != '\0') {}
            c--;
            for(; c > 0; c--) {
                SDL_RenderCopy(renderer, textRenderer->font, &(SDL_Rect){ 5*_calculate_char_offset(text[c]), 0, 5, 9 }, &(SDL_Rect){ anchor->x+anchor->w-5*c, anchor->y, 5, 9 });
            }
            break;
        case CENTER:
            c = 0;
            int count = 0;
            while(text[c++] != '\0') {
                if(text[c] == '\xff') {c+=3; continue;}
                count++;
            }
            count--;
            int gap = (anchor->w-5*count)/2;
            int col = 0;
            for(c = 0; text[c] != '\0'; c++) {
                if(text[c] == '\xff') {
                    // SDL_SetTextureColorMod(textRenderer->font, 255, 255, 255);
                    char *color = text+c+1;
                    SDL_SetTextureColorMod(textRenderer->font, color[0], color[1], color[2]);
                    c += 3;
                } else {
                    SDL_RenderCopy(renderer, textRenderer->font, &(SDL_Rect){ 5*_calculate_char_offset(text[c]), 0, 5, 9 }, &(SDL_Rect){ gap+anchor->x+5*(col++), anchor->y, 5, 9 });
                }
            }
            break;
    }
}