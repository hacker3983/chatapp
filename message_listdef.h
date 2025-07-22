#ifndef _MESSAGE_LISTDEF_H
#define _MESSAGE_LISTDEF_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct message {
    TTF_Font* font;
    int font_size;

    // Our actual message properties
    char* text;
    SDL_Rect text_canvas;
    SDL_Color text_color;

    // This will be our background for our message
    int padding_x, padding_y,
        space_y;
    SDL_Rect canvas;
    SDL_Color canvas_color;
} message_t;

typedef struct message_list {
    int start_x, start_y;
    bool scrolled;
    SDL_Rect viewport;
    message_t* messages;
    size_t render_pos, message_count;
} message_list_t;
#endif
