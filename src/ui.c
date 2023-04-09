#include "main.h"
#include <string.h>
#include <stdio.h>

void ui_create_button(struct UIButton *button, char *text, struct UIColor neutralColor, struct UIColor hoverColor, int x, int y, int w, int h, UIButtonCallback *on_click) {
    button->text = text;
    button->neutralColor = neutralColor;
    button->hoverColor = hoverColor;
    button->x = x;
    button->y = y;
    button->w = w;
    button->h = h;
    button->on_click = on_click;
    button->clicked = 0;
}
void ui_create_text(struct UIText *uitext, char *text, SDL_Rect anchor, enum TextAlign align) {
    uitext->text = text;
    uitext->anchor = anchor;
    uitext->align = align;
}

void ui_draw_button(struct UIButton *button, struct AppContext *context) {
    SDL_Rect buttonBounds = { button->x, button->y, button->w, button->h };
    if(SDL_PointInRect(&(SDL_Point){ context->mouseMove.x, context->mouseMove.y }, &buttonBounds)) { // mouse is over button
        SDL_SetRenderDrawColor(mainRenderer,
            button->hoverColor.r,
            button->hoverColor.g,
            button->hoverColor.b,
            button->hoverColor.a
        );
        if(context->mouseClick.button == SDL_BUTTON_LEFT && context->mouseClick.state == SDL_PRESSED) {
            button->clicked = 1;
        }
        if(button->clicked && context->mouseClick.button == SDL_BUTTON_LEFT && context->mouseClick.state == SDL_RELEASED) {
            button->clicked = 0;
            button->on_click((struct UIButtonCallbackInfo){
                button, context
            });
        }
    } else {
        SDL_SetRenderDrawColor(mainRenderer,
            button->neutralColor.r,
            button->neutralColor.g,
            button->neutralColor.b,
            button->neutralColor.a
        );
        button->clicked = 0;
    }
    
    SDL_RenderFillRect(mainRenderer, &buttonBounds);
    int vgap = (button->h - 9)/ 2;
    text_write(textRenderer, button->text, mainRenderer, &(SDL_Rect){ button->x, button->y+vgap, button->w, button->h }, CENTER);
}

// Insert a button into the scene and increment the button counter
size_t insert_button(struct AppContext *context, char *text, struct UIColor neutralColor, struct UIColor hoverColor, int x, int y, int w, int h, UIButtonCallback *on_click) {
    char *textc = malloc(1024*sizeof(char));
    strncpy(textc, text, 1024);

    if(context->scene->freeButtonIndex >= 0) {
        int insertedAt = context->scene->freeButtonIndex;
        // context->scene->buttons[insertedAt] = malloc(sizeof(struct UIButton));
        ui_create_button(
            context->scene->buttons[insertedAt],
            textc,
            neutralColor,
            hoverColor,
            x,y,w,h,
            on_click
        );
        context->scene->numButtons++;
        context->scene->freeButtonIndex = -1;
        return insertedAt;
    } else {
        context->scene->buttons[context->scene->numButtons++] = malloc(sizeof(struct UIButton));
        ui_create_button(
            context->scene->buttons[context->scene->numButtons-1],
            textc,
            neutralColor,
            hoverColor,
            x,y,w,h,
            on_click
        );
        return context->scene->numButtons-1;
    }
    
}
size_t insert_text(struct AppContext *context, char *text, SDL_Rect anchor, enum TextAlign align) {
    char *textc = malloc(4096*sizeof(char));
    strncpy(textc, text, 4096);

    if(context->scene->freeTextIndex >= 0) {
        int insertedAt = context->scene->freeTextIndex;
        context->scene->texts[insertedAt] = malloc(sizeof(struct UIText));
        ui_create_text(
            context->scene->texts[insertedAt],
            textc,
            anchor,
            align
        );
        context->scene->numTexts++;
        context->scene->freeTextIndex = -1;
        return insertedAt;
    } else {
        context->scene->texts[context->scene->numTexts++] = malloc(sizeof(struct UIText));
        ui_create_text(
            context->scene->texts[context->scene->numTexts-1],
            textc,
            anchor,
            align
        );
        return context->scene->numTexts-1;
    }
    
}

void delete_text(struct AppContext *context, size_t index) {
    // if(index < context->scene->numTexts - 1)
    //     context->scene->freeTextIndex = index;
    // context->scene->numTexts--;
    free(context->scene->texts[index]->text);
    free(context->scene->texts[index]);
    context->scene->texts[index] = NULL;
}
void delete_button(struct AppContext *context, size_t index) {
    // context->scene->freeButtonIndex = index;
    // context->scene->numButtons--;
    free(context->scene->buttons[index]->text);
    free(context->scene->buttons[index]);
    context->scene->buttons[index] = NULL;
}

// TODO: make gap work
void insert_button_row(struct AppContext *context, size_t num, struct UIButton *buttons, SDL_Rect anchor, int gap) {
    int button_size = 2*(anchor.w/num+1)/2; // round up
    for(size_t i = 0; i < num; i++) {
        struct UIButton button_info = buttons[i];
        insert_button(context, 
            button_info.text, 
            button_info.neutralColor,
            button_info.hoverColor,
            anchor.x+i*button_size,anchor.y,button_size,anchor.h,
            button_info.on_click
        );
    }
}