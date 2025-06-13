#include "message_list.h"
#include "app_strings.h"

void message_init(message_t* message) {
    memset(message, 0, sizeof(message_t));
}

void message_list_init(message_list_t* message_list) {
    message_list->messages = NULL;
    message_list->message_count = 0;
}

void message_list_setstartpos(message_list_t* message_list, int start_x, int start_y) {
    message_list->start_x = start_x,
    message_list->start_y = start_y;
}

void message_list_add(message_list_t* message_list,
    TTF_Font* font, int font_size, const char* message,
    SDL_Color message_color, int padding_x,
    int padding_y, int space_y,
    SDL_Color canvas_color) {
    if(!message) {
        return;
    }
    size_t new_count = message_list->message_count + 1;
    message_t* new_messages = realloc(message_list->messages, new_count
        * sizeof(message_t));
    if(!new_messages) {
        return;
    }
    new_messages[new_count-1].font = font;
    new_messages[new_count-1].font_size = font_size;
    new_messages[new_count-1].text = string_dupstr(message);
    TTF_SizeText(font, message, &new_messages[new_count-1].text_canvas.w,
        &new_messages[new_count-1].text_canvas.h);
    new_messages[new_count-1].canvas = new_messages[new_count-1].text_canvas;
    new_messages[new_count-1].canvas.w += padding_x * 2;
    new_messages[new_count-1].canvas.h += padding_y * 2;
    new_messages[new_count-1].space_y = space_y;
    new_messages[new_count-1].text_color = message_color;
    new_messages[new_count-1].canvas_color = canvas_color;
    message_list->messages = new_messages;
    message_list->message_count = new_count;
}

void message_list_display(app_t* app, message_list_t* message_list) {
    if(!message_list->messages) {
        return;
    }
    int start_x = message_list->start_x, start_y = message_list->start_y;
    for(size_t i=0;i<message_list->message_count;i++) {
        SDL_Rect* canvas = &message_list->messages[i].canvas,
            *text_canvas = &message_list->messages[i].text_canvas;
        canvas->x = start_x,
        canvas->y = start_y;
    
        // Display our message canvas
        SDL_SetRenderDrawColor(app->renderer,
            color_toparam(message_list->messages[i].canvas_color));
        SDL_RenderDrawRect(app->renderer, canvas);
        SDL_RenderFillRect(app->renderer, canvas);

        // Center our message onto the message canvas
        text_canvas->x = canvas->x + (canvas->w - text_canvas->w) / 2;
        text_canvas->y = canvas->y + (canvas->h - text_canvas->h) / 2;

        // Display our actual message
        SDL_Surface* message_surface = TTF_RenderText_Blended(message_list->messages[i].font,
            message_list->messages[i].text,
            message_list->messages[i].text_color);
        SDL_Texture* message_texture = SDL_CreateTextureFromSurface(app->renderer, message_surface);
        SDL_RenderCopy(app->renderer, message_texture, NULL, text_canvas);
        SDL_FreeSurface(message_surface);
        SDL_DestroyTexture(message_texture);
        start_y += canvas->h + message_list->messages[i].space_y;
    }
}

void message_destroy(message_t* message) {
    free(message->text);
    message_init(message);
}

void message_list_destroy(message_list_t* message_list) {
    for(size_t i=0;i<message_list->message_count;i++) {
        message_destroy(&message_list->messages[i]);
    }
    message_list_init(message_list);
}