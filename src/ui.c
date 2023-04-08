#include "ui.h"

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
