#include "inputbox.h"
#include "app_strings.h"

void inputbox_init(inputbox_t* inputbox, TTF_Font* font, int font_size,
    SDL_Color text_color, int width, int height, SDL_Color color,
    int cursor_width, int cursor_height, SDL_Color cursor_color,
    double cursor_timeoutsecs,
    SDL_Color selection_color) {
    inputbox->font = font;
    inputbox->font_size = font_size;
    inputbox->canvas.w = width,
    inputbox->canvas.h = height;
    inputbox->color = color;
    inputbox->text_color = text_color;
    inputbox->cursor.w = cursor_width;
    inputbox->cursor.h = cursor_height;
    inputbox->cursor_color = cursor_color;
    inputbox->cursor_blink = true;
    inputbox->cursor_show = true;
    inputbox->cursor_timeoutsecs = cursor_timeoutsecs;
    inputbox->selection_color = selection_color;
}

inputbox_t inputbox_create(TTF_Font* font, int font_size,
    SDL_Color text_color, int width, int height, SDL_Color color,
    int cursor_width, int cursor_height, SDL_Color cursor_color,
    int cursor_timeoutsecs,
    SDL_Color selection_color) {
    inputbox_t new_inputbox = {0};
    inputbox_init(&new_inputbox, font, font_size, text_color,
        width, height, color, cursor_width, cursor_height, cursor_color, cursor_timeoutsecs, selection_color);
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
    TTF_SizeUTF8(inputbox->font, utf8_char, &utf8_charwidth,
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
        char* utf8_char = string_getutf8char(data, &i, datalen);
        inputbox_addinputchar(inputbox, utf8_char);
    }
    inputbox_getprimarydata(inputbox);
}


void inputbox_addselectionleft(inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    if(!input->characters || !input->cursor_pos) {
        return;
    }
    if(input->selection_direction == INPUT_SELECTIONRIGHT && input->selection_count) {
        input->cursor_pos--;
        input->selection_end--;
        input->selection_count--;
        return;
    }
    input->selection_direction = INPUT_SELECTIONLEFT;
    if(!input->selection_count) {
        input->selection_end = input->cursor_pos;
    }
    input->cursor_pos--;
    input->selection_start = input->cursor_pos;
    input->selection_count++;
}

void inputbox_addselectionright(inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    if(!input->characters || input->cursor_pos >= input->character_count) {
        return;
    }
    if(input->selection_direction == INPUT_SELECTIONLEFT && input->selection_count) {
        input->cursor_pos++;
        input->selection_start++;
        input->selection_count--;
        return;
    }
    input->selection_direction = INPUT_SELECTIONRIGHT;
    if(!input->selection_count) {
        input->selection_start = input->cursor_pos;
    }
    input->cursor_pos++;
    input->selection_end = input->cursor_pos;
    input->selection_count++;
}

void inputbox_addselection(inputbox_t* inputbox, int direction) {
    if(direction == INPUT_SELECTIONLEFT) {
        inputbox_addselectionleft(inputbox);
    } else {
        inputbox_addselectionright(inputbox);
    }
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
    inputdata_t* input = &inputbox->input;
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

        SDL_Surface* text_surface = TTF_RenderUTF8_Blended(inputbox->font,
            inputbox->input.characters[i].utf8_char,
            inputbox->text_color
        );
        if(input->selection_count &&
            i < input->selection_end && i >= input->selection_start) {
            SDL_SetRenderDrawColor(app->renderer, color_toparam(inputbox->selection_color));
            SDL_RenderDrawRect(app->renderer, &input->characters[i].canvas);
            SDL_RenderFillRect(app->renderer, &input->characters[i].canvas);
        }
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

void inputbox_resetcursorblink(inputbox_t* inputbox) {
    inputbox->cursor_timeout = 0;
    inputbox->cursor_blinking = false;
}

void inputbox_rendercursor(app_t* app, inputbox_t* inputbox) {
    if(!inputbox->cursor_show) {
        return;
    }
    inputbox->cursor.y = inputbox->canvas.y + (inputbox->canvas.h - inputbox->cursor.h) / 2;
    if(inputbox->cursor_blink) {
        if(!inputbox->cursor_timeout) {
            inputbox->cursor_timeout = SDL_GetTicks64() + (inputbox->cursor_timeoutsecs * 1000);
        } else if(inputbox->cursor_blinking) {
            if(SDL_GetTicks64() >= inputbox->cursor_timeout) {
                inputbox->cursor_timeout = 0;
                inputbox->cursor_blinking = false;
            }
            return;
        } else if(!inputbox->cursor_blinking && SDL_GetTicks64() >= inputbox->cursor_timeout) {
            inputbox->cursor_timeout = 0;
            inputbox->cursor_blinking = true;
        }
    }
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
}

void inputbox_display(app_t* app, inputbox_t* inputbox) {
    SDL_SetRenderDrawColor(app->renderer, color_toparam(inputbox->color));
    SDL_RenderDrawRect(app->renderer, &inputbox->canvas);
    SDL_RenderFillRect(app->renderer, &inputbox->canvas);
    //printf("inputbox->data = %s\n", inputbox->data);
    inputbox_rendercharacters(app, inputbox);
    inputbox_rendercursor(app, inputbox);
}

void inputbox_selectall(inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    input->selection_start = 0;
    input->selection_end = input->character_count;
    input->cursor_pos = input->character_count;
    input->selection_direction = INPUT_SELECTIONRIGHT;
    input->selection_count = input->character_count;
}

bool inputbox_deleteselection(inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    if(input->selection_count) {
        input->cursor_pos = input->selection_end;
        for(size_t i=input->selection_end;i>input->selection_start;i--) {
            inputbox_backspace(inputbox);
        }
        inputbox_clearselection(inputbox);
        return true;
    }
    return false;
}

void inputbox_handle_events(app_t* app, inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    switch(app->e.type) {
        case SDL_TEXTINPUT:
            inputbox_deleteselection(inputbox);
            inputbox_addinputdata(inputbox, app->e.text.text);
            inputbox_resetcursorblink(inputbox);
            break;
        case SDL_KEYDOWN:
            if(SDL_GetModState() & KMOD_SHIFT) {
                switch(app->e.key.keysym.sym) {
                    case SDLK_LEFT:
                        inputbox_addselection(inputbox, INPUT_SELECTIONLEFT);
                        inputbox_printselection(inputbox);
                        break;
                    case SDLK_RIGHT:
                        inputbox_addselection(inputbox, INPUT_SELECTIONRIGHT);
                        inputbox_printselection(inputbox);
                        break;
                }
                break;
            } else if(SDL_GetModState() & KMOD_CTRL) {
                switch(app->e.key.keysym.sym) {
                    case SDLK_a:
                        inputbox_selectall(inputbox);
                        break;
                    case SDLK_c:
                        char* selection_data = inputbox_getselectiondata(inputbox);
                        if(selection_data) {
                            SDL_SetClipboardText(selection_data);
                        }
                        free(selection_data);
                        break;
                    case SDLK_v:
                        char* clipboard_data = SDL_GetClipboardText();
                        if(clipboard_data) {
                            inputbox_deleteselection(inputbox);
                            inputbox_addinputdata(inputbox, clipboard_data);
                        }
                        free(clipboard_data);
                        break;
                }
            }
            switch(app->e.key.keysym.sym) {
                case SDLK_LEFT:
                    if(input->selection_count) {
                        input->cursor_pos = input->selection_start;
                        inputbox_clearselection(inputbox);
                        break;
                    }
                    if(inputbox->input.cursor_pos) {
                        inputbox->input.cursor_pos--;
                    }
                    inputbox_resetcursorblink(inputbox);
                    break;
                case SDLK_RIGHT:
                    if(input->selection_count) {
                        input->cursor_pos = input->selection_end;
                        inputbox_clearselection(inputbox);
                        break;
                    }
                    if(inputbox->input.cursor_pos < inputbox->input.character_count) {
                        inputbox->input.cursor_pos++;
                    }
                    inputbox_resetcursorblink(inputbox);
                    break;
                case SDLK_BACKSPACE:
                    if(inputbox_deleteselection(inputbox)) {
                        break;
                    }
                    inputbox_backspace(inputbox);
                    inputbox_resetcursorblink(inputbox);
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

char* inputbox_getselectiondata(inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    char* selection_data = NULL;
    for(size_t i=input->selection_start;i<input->selection_end;i++) {
        string_concatstr(&selection_data, input->characters[i].utf8_char);
    }
    return selection_data;
}

void inputbox_printselection(inputbox_t* inputbox) {
    inputdata_t* input = &inputbox->input;
    printf("Selection data is: ");
    for(size_t i=input->selection_start;i<input->selection_end;i++) {
        printf("%s", input->characters[i].utf8_char);
    }
    printf("\n");
}

void inputbox_clearselection(inputbox_t* inputbox) {
    inputbox->input.selection_start = 0;
    inputbox->input.selection_end = 0;
    inputbox->input.selection_count = 0;
}

void inputbox_clear(inputbox_t* inputbox) {
    inputbox_clearselection(inputbox);
    for(size_t i=0;i<inputbox->input.character_count;i++) {
        free(inputbox->input.characters[i].utf8_char);
    }
    free(inputbox->input.characters);
    inputbox->input.characters = NULL;
    inputbox->input.character_count = 0;
    inputbox->input.cursor_pos = 0;
    free(inputbox->data);
    inputbox->data = NULL;
    inputbox->enter_pressed = false;
}

void inputbox_destroy(inputbox_t* inputbox) {
    inputbox_clear(inputbox);
}