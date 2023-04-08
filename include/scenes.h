#ifndef __BC23_SCENES
#define __BC23_SCENES

#include "events.h"

void _initINIT(struct AppContext *context);
void _initEDIT(struct AppContext *context);
void _initPLAY(struct AppContext *context);

void __test_button_callback(struct UIButtonCallbackInfo info);
void ui_dummy_callback(struct UIButtonCallbackInfo info);

void scenes_init_list_programs(struct UIButtonCallbackInfo info);

#endif