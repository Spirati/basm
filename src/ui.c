#include "main.h"

void ui_create_button(struct UIButton *button, char *text, struct UIColor neutralColor, struct UIColor hoverColor, int x, int y, int w, int h, void (*on_click)(void)) {
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
            button->on_click();
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
    text_write(textRenderer, button->text, mainRenderer, &(SDL_Rect){ button->x, button->y, button->w, button->h }, CENTER);
}
