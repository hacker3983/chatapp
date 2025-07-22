#ifndef _APP_H
#define _APP_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "message_listdef.h"
#include "inputboxdef.h"
#include "chatsock_def.h"
#define color_toparam(color) color.r, color.g, color.b, color.a

enum app_scrolltype {
	APP_SCROLLUP,
	APP_SCROLLDOWN
};

typedef struct app {
    int win_width, win_height,
	mouse_x, mouse_y;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event e;

    // server socket
    chatsock_t server_sock;

    // input box
    inputbox_t inputbox;

    // Our message list
    message_list_t message_list;

    int scroll_type;
    bool scroll;

    TTF_Font* inputbox_font;
    TTF_Font* font;
    int font_size;
    bool quit;
} app_t;

void app_init();
void app_create(app_t* app);
bool app_rect_hover_horizontal(app_t* app, SDL_Rect rect);
bool app_rect_hover_vertical(app_t* app, SDL_Rect rect);
bool app_rect_hover(app_t* app, SDL_Rect rect);
void app_getwindowsize(app_t* app);
void app_run(app_t* app);
void app_destroy(app_t* app);
#endif
