#ifndef __BC23_EVENTS
#define __BC23_EVENTS

#include "SDL/SDL.h"

#define MAX_BUTTONS 32
#define MAX_MESSAGES 32

void event_loop(void);

struct UIButton;
struct UIText {
    char *text;
    SDL_Rect anchor;
    enum TextAlign align;
};

enum AppState {
    INIT,
    EDIT,
    PLAY,
    APP_ERROR
};

struct UIScene {
    struct UIButton **buttons;
    struct UIText **texts;
    size_t numButtons;
    size_t numTexts;

    int freeButtonIndex;
    int freeTextIndex;
};

struct InitSceneInfo {
    int warning_message_index;
    int toolbar_end;
};
struct PlaySceneInfo {
    size_t dummy;
};
struct EditSceneInfo {
    char *filename;
    char *fulltext;
    char *showntext;
    int scrollStart;
    int scrollEnd;
    size_t filelength;
    size_t linecount;
};
struct ErrorSceneInfo {
    char *error;
};

union SceneInfo {
    struct InitSceneInfo *init;
    struct EditSceneInfo *edit;
    struct PlaySceneInfo *play;
    struct ErrorSceneInfo *error;
};

struct AppContext {
    enum AppState currentState;
    union SceneInfo *scene_info;
    struct UIScene *scene;
    SDL_MouseMotionEvent mouseMove;
    SDL_MouseButtonEvent mouseClick;
    int shutdown;
};

struct UIButtonCallbackInfo {
    struct UIButton *button;
    struct AppContext *context;
};

typedef void UIButtonCallback(struct UIButtonCallbackInfo);

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
    UIButtonCallback *on_click;
    int clicked;
};

size_t insert_button(struct AppContext *context, char *text, struct UIColor neutralColor, struct UIColor hoverColor, int x, int y, int w, int h, UIButtonCallback *on_click);
void insert_button_row(struct AppContext *context, size_t num, struct UIButton *buttons, SDL_Rect anchor, int gap);

size_t insert_text(struct AppContext *context, char *text, SDL_Rect anchor, enum TextAlign align);

void delete_text(struct AppContext *context, size_t index);
void delete_button(struct AppContext *context, size_t index);

void ui_create_button(struct UIButton *button, char *text, struct UIColor neutralColor, struct UIColor hoverColor, int x, int y, int w, int h, UIButtonCallback *callback);
void ui_create_text(struct UIText *uitext, char *text, SDL_Rect anchor, enum TextAlign align);
void ui_draw_button(struct UIButton *button, struct AppContext *context);

void event_switch_scene(struct AppContext *context, enum AppState state, union SceneInfo *pass);

#endif