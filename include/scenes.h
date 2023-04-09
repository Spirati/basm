#ifndef __BC23_SCENES
#define __BC23_SCENES

#include "events.h"

int _initINIT(struct AppContext *context);
int _initEDIT(struct AppContext *context);
int _initPLAY(struct AppContext *context);
int _initERROR(struct AppContext *context);

union SceneInfo *event_generate_error(char *error);

void __test_button_callback(struct UIButtonCallbackInfo info);
void ui_dummy_callback(struct UIButtonCallbackInfo info);

void scenes_init_list_programs(struct UIButtonCallbackInfo info);

#endif