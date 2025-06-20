#ifndef _INPUTBOXDEF_H
#define _INPUTBOXDEF_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum input_selectiondirection {
    INPUT_SELECTIONLEFT,
    INPUT_SELECTIONRIGHT
};

typedef struct input_character {
    char* utf8_char;
    size_t utf8_charlen;
    SDL_Rect canvas;
} inputchar_t;

typedef struct input_data {
    inputchar_t* characters;
    size_t selection_start, selection_end, selection_count;
    int selection_direction;
    size_t cursor_pos, character_count;
} inputdata_t;

typedef struct inputbox {
    TTF_Font* font;
    int font_size;

    double cursor_timeoutsecs;
    uint64_t cursor_timeout;
    SDL_Rect cursor;
    SDL_Color cursor_color;
    bool cursor_show, cursor_blink, cursor_blinking;

    inputdata_t input;
    char* data;
    SDL_Color selection_color;
    SDL_Color text_color;

    SDL_Color color;
    SDL_Rect canvas;
    bool enter_pressed;
} inputbox_t;
#endif