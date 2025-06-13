#ifndef _APP_H
#define _APP_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "message_listdef.h"
#include "inputboxdef.h"
#define color_toparam(color) color.r, color.g, color.b, color.a

typedef struct app {
    int win_width, win_height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event e;

    // input box
    inputbox_t inputbox;

    // Our message list
    message_list_t message_list;

    TTF_Font* font;
    int font_size;
    bool quit;
} app_t;

void app_init();
void app_create(app_t* app);
void app_getwindowsize(app_t* app);
void app_run(app_t* app);
void app_destroy(app_t* app);
#endif