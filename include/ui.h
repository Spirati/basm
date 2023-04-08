#ifndef __BC23_UI
#define __BC23_UI

struct UIColor {
    char r; char g; char b; char a;
};

struct UIButton {
    char *text;
    struct UIColor neutralColor;
    struct UIColor hoverColor;
    int x;
    int y;
    int w;
    int h;
    void (*on_click)(void);
    int clicked;
};

struct UIScene {
    struct UIButton *buttons;
};

void ui_create_button(struct UIButton *button, char *text, struct UIColor neutralColor, struct UIColor hoverColor, int x, int y, int w, int h, void (*on_click)(void));

#endif