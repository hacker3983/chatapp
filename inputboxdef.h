#ifndef _INPUTBOXDEF_H
#define _INPUTBOXDEF_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct input_character {
    char* utf8_char;
    size_t utf8_charlen;
    SDL_Rect canvas;
} inputchar_t;

typedef struct input_data {
    inputchar_t* characters;
    size_t cursor_pos, character_count;
} inputdata_t;

typedef struct inputbox {
    TTF_Font* font;
    int font_size;

    SDL_Rect cursor;
    SDL_Color cursor_color;

    inputdata_t input;
    char* data;
    SDL_Color text_color;

    SDL_Color color;
    SDL_Rect canvas;
    bool enter_pressed;
} inputbox_t;
#endif