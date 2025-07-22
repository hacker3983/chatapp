#ifndef _MESSAGE_LIST_H
#define _MESSAGE_LIST_H
#include "app.h"

void message_init(message_t* message);
void message_destroy(message_t* message);

void message_list_init(message_list_t* message_list);
void message_list_setstartpos(message_list_t* message_list, int start_x, int start_y);
void message_list_setviewport(message_list_t* message_list, SDL_Rect viewport);
void message_list_add(message_list_t* message_list,
    TTF_Font* font, int font_size, const char* message,
    SDL_Color message_color, int padding_x, int padding_y,
    int space_y,
    SDL_Color canvas_color);
void message_list_display(app_t* app, message_list_t* message_list);
void message_list_performscroll(app_t* app, message_list_t* message_list);
void message_list_destroy(message_list_t* message_list);
#endif
