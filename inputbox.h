#ifndef _INPUTBOX_H
#define _INPUTBOX_H
#include "app.h"


void inputbox_init(inputbox_t* inputbox, TTF_Font* font, int font_size,
    SDL_Color text_color, int width, int height, SDL_Color color,
    int cursor_width, int cursor_height, SDL_Color cursor_color,
    double cursor_timeoutsecs,
    SDL_Color selection_color);
inputbox_t inputbox_create(TTF_Font* font, int font_size,
    SDL_Color text_color, int width, int height, SDL_Color color,
    int cursor_width, int cursor_height, SDL_Color cursor_color,
    int cursor_timeoutsecs,
    SDL_Color selection_color);
void inputbox_setfontsize(inputbox_t* inputbox);
void inputbox_addinputchar(inputbox_t* inputbox, char* utf8_char);
void inputbox_addinputdata(inputbox_t* inputbox, const char* data);
char* inputbox_getinputdata(inputbox_t* inputbox);
void inputbox_getprimarydata(inputbox_t* inputbox);
void inputbox_resetcursorblink(inputbox_t* inputbox);
void inputbox_backspace(inputbox_t* inputbox);
void inputbox_addselectionleft(inputbox_t* inputbox);
void inputbox_addselectionright(inputbox_t* inputbox);
void inputbox_addselection(inputbox_t* inputbox, int direction);
char* inputbox_getselectiondata(inputbox_t* inputbox);
void inputbox_printselection(inputbox_t* inputbox);
void inputbox_clearselection(inputbox_t* inputbox);
void inputbox_handle_events(app_t* app, inputbox_t* inputbox);
void inputbox_display(app_t* app, inputbox_t* inputbox);
void inputbox_clear(inputbox_t* inputbox);
void inputbox_destroy(inputbox_t* inputbox);
#endif