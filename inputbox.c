#include "inputbox.h"
#include "app_strings.h"

void inputbox_init(inputbox_t* inputbox, TTF_Font* font, int font_size,
    SDL_Color text_color, int width, int height, SDL_Color color,
    int cursor_width, int cursor_height, SDL_Color cursor_color) {
    inputbox->font = font;
    inputbox->font_size = font_size;
    inputbox->canvas.w = width,
    inputbox->canvas.h = height;
    inputbox->color = color;
    inputbox->text_color = text_color;
    inputbox->cursor.w = cursor_width;
    inputbox->cursor.h = cursor_height;
    inputbox->cursor_color = cursor_color;
}

inputbox_t inputbox_create(TTF_Font* font, int font_size,
    SDL_Color text_color, int width, int height, SDL_Color color,
    int cursor_width, int cursor_height, SDL_Color cursor_color) {
    inputbox_t new_inputbox = {0};
    inputbox_init(&new_inputbox, font, font_size, text_color,
        width, height, color, cursor_width, cursor_height, cursor_color);
    return new_inputbox;
}

void inputbox_setfontsize(inputbox_t* inputbox) {
    TTF_SetFontSize(inputbox->font, inputbox->font_size);
}

void inputbox_addinputchar(inputbox_t* inputbox, char* utf8_char) {
    if(!utf8_char) {
        return;
    }
    size_t new_charcount = inputbox->input.character_count + 1;
    inputchar_t* new_characters = realloc(inputbox->input.characters, new_charcount
        * sizeof(inputchar_t));
    if(!new_characters) {
        return;
    }
    int utf8_charwidth = 0,
        utf8_charheight = 0;
    inputbox_setfontsize(inputbox);
    TTF_SizeText(inputbox->font, utf8_char, &utf8_charwidth,
        &utf8_charheight);
    size_t utf8_charlen = strlen(utf8_char);
    new_characters[new_charcount-1].utf8_char = utf8_char;
    new_characters[new_charcount-1].utf8_charlen = utf8_charlen;
    new_characters[new_charcount-1].canvas.w = utf8_charwidth;
    new_characters[new_charcount-1].canvas.h = utf8_charheight;
    for(size_t i=new_charcount-1;i>inputbox->input.cursor_pos;i--) {
        inputchar_t current_char = new_characters[i];
        new_characters[i] = new_characters[i-1],
        new_characters[i-1] = current_char;
    }
    inputbox->input.cursor_pos++;
    inputbox->input.characters = new_characters;
    inputbox->input.character_count = new_charcount;
}

void inputbox_addinputdata(inputbox_t* inputbox, const char* data) {
    size_t datalen = strlen(data);
    for(size_t i=0;i<datalen;i++) {
        char* utf8_char = string_dupstr((const char[2]){data[i], 0});
        inputbox_addinputchar(inputbox, utf8_char);
    }
    inputbox_getprimarydata(inputbox);
}

char* inputbox_getinputdata(inputbox_t* inputbox) {
    char* new_string = NULL;
    for(size_t i=0;i<inputbox->input.character_count;i++) {
        string_concatstr(&new_string, inputbox->input.characters[i].utf8_char);
    }
    return new_string;
}

void inputbox_getprimarydata(inputbox_t* inputbox) {
    free(inputbox->data);
    inputbox->data = inputbox_getinputdata(inputbox);
}

void inputbox_rendercharacters(app_t* app, inputbox_t* inputbox) {
    int start_x = inputbox->canvas.x + 1,
        start_y = inputbox->canvas.y;
    inputbox->cursor.x = start_x;
    inputbox_setfontsize(inputbox);
    for(size_t i=0;i<inputbox->input.character_count;i++) {
        if(i == inputbox->input.cursor_pos) {
            inputbox->cursor.x = start_x;
            start_x += inputbox->cursor.w;
        }
        inputbox->input.characters[i].canvas.x = start_x;
        inputbox->input.characters[i].canvas.y = start_y;

        SDL_Surface* text_surface = TTF_RenderText_Blended(inputbox->font,
            inputbox->input.characters[i].utf8_char,
            inputbox->text_color
        );
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(app->renderer, text_surface);
        SDL_RenderCopy(app->renderer, text_texture, NULL, &inputbox->input.characters[i].canvas);
        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);
        start_x += inputbox->input.characters[i].canvas.w;
    }
    size_t last_characterindex = (inputbox->input.character_count) ? inputbox->input.character_count-1 : 0;
    if(inputbox->input.characters &&
        inputbox->input.cursor_pos == inputbox->input.character_count) {
        inputbox->cursor.x = inputbox->input.characters[last_characterindex].canvas.x +
            inputbox->input.characters[last_characterindex].canvas.w;
    }
}

void inputbox_rendercursor(app_t* app, inputbox_t* inputbox) {
    inputbox->cursor.y = inputbox->canvas.y + (inputbox->canvas.h - inputbox->cursor.h) / 2;
    SDL_SetRenderDrawColor(app->renderer, color_toparam(inputbox->cursor_color));
    SDL_RenderDrawRect(app->renderer, &inputbox->cursor);
    SDL_RenderFillRect(app->renderer, &inputbox->cursor);
}

void inputbox_backspace(inputbox_t* inputbox) {
    if(!inputbox->input.characters ||
        !inputbox->input.cursor_pos) {
        return;
    }
    /*
    ab
    */
    size_t new_count = inputbox->input.character_count-1;
    for(size_t i=inputbox->input.cursor_pos-1;i<new_count;i++) {
        printf("Shifting to the end at index %zu\n", i);
        inputchar_t current_char = inputbox->input.characters[i];
        inputbox->input.characters[i] = inputbox->input.characters[i+1];
        inputbox->input.characters[i+1] = current_char;
    }
    inputbox->input.cursor_pos--;
    if(!new_count) {
        inputbox_clear(inputbox);
        return;
    }
    inputbox->input.characters = realloc(inputbox->input.characters,
        new_count * sizeof(inputchar_t));
    inputbox->input.character_count = new_count;
    inputbox_getprimarydata(inputbox);
    printf("input data after backspace: %s\n", inputbox->data);
}

void inputbox_display(app_t* app, inputbox_t* inputbox) {
    SDL_SetRenderDrawColor(app->renderer, color_toparam(inputbox->color));
    SDL_RenderDrawRect(app->renderer, &inputbox->canvas);
    SDL_RenderFillRect(app->renderer, &inputbox->canvas);
    //printf("inputbox->data = %s\n", inputbox->data);
    inputbox_rendercharacters(app, inputbox);
    inputbox_rendercursor(app, inputbox);
}


void inputbox_handle_events(app_t* app, inputbox_t* inputbox) {
    switch(app->e.type) {
        case SDL_TEXTINPUT:
            inputbox_addinputdata(inputbox, app->e.text.text);
            break;
        case SDL_KEYDOWN:
            switch(app->e.key.keysym.sym) {
                case SDLK_LEFT:
                    if(inputbox->input.cursor_pos) {
                        inputbox->input.cursor_pos--;
                    }
                    break;
                case SDLK_RIGHT:
                    if(inputbox->input.cursor_pos < inputbox->input.character_count) {
                        inputbox->input.cursor_pos++;
                    }
                    break;
                case SDLK_BACKSPACE:
                    printf("Calling backspace\n");
                    inputbox_backspace(inputbox);
                    printf("Done calling backspace\n");
                    break;
            }
            break;
        case SDL_KEYUP:
            switch(app->e.key.keysym.sym) {
                case SDLK_RETURN:
                    inputbox->enter_pressed = true;
                    break;
            }
            break;
    }
}

void inputbox_clear(inputbox_t* inputbox) {
    for(size_t i=0;i<inputbox->input.character_count;i++) {
        free(inputbox->input.characters[i].utf8_char);
    }
    free(inputbox->input.characters);
    inputbox->input.characters = NULL;
    inputbox->input.character_count = 0;
    inputbox->input.cursor_pos = 0;
    free(inputbox->data);
    inputbox->data = NULL;
}

void inputbox_destroy(inputbox_t* inputbox) {
    inputbox_clear(inputbox);
}