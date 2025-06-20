#include "app.h"
#include "appconfig.h"
#include "message_list.h"
#include "app_strings.h"
#include "inputbox.h"

void app_init() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL\n");
    }
	if(TTF_Init() < 0) {
        fprintf(stderr, "Failed to initialize SDL ttf\n");
    }
}
void app_create(app_t* app) {
    app_init();
    app->window = SDL_CreateWindow("Chat Application in C",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    app->font = TTF_OpenFont("Roboto-Italic-VariableFont_wdth,wght.ttf",
		FONT_SIZE);
	app->font_size = FONT_SIZE;
	app->inputbox_font = TTF_OpenFont("Symbola.ttf",
		FONT_SIZE);
}

void app_getwindowsize(app_t* app) {
    SDL_GetWindowSize(app->window, &app->win_width, &app->win_height);
}

void app_setwindowcolor(app_t* app, SDL_Color window_color) {
	SDL_SetRenderDrawColor(app->renderer, color_toparam(window_color));
	SDL_RenderClear(app->renderer);
}

void app_run(app_t* app) {
    if(!app->font) {
        printf("Failed to open font\n");
    }
    if(TTF_SizeText(app->font, header_text, &header_w, &header_h) < 0) {
        printf("Failed to get the size of the text\n");
    }
    SDL_Rect header_textcanvas = {
		.x = 0,
		.y = 0,
		.w = (const int)header_w,
		.h = (const int)header_h
    };
    SDL_Rect header = {
		.x = 0,
		.y = 0,
		.w = (const int)header_w + 40,
		.h = (const int)header_h + 40
    };
    SDL_Color header_color = {0};

    SDL_Rect message_canvas = {
	    .x = 0,
		.y = header.y + header.h + 10,
		.w = 0,
		.h = 0
    };
    SDL_Surface* header_textsurface = TTF_RenderText_Blended(app->font, header_text,
	    	header_textcolor);
    if(header_textsurface) {
        printf("The header textsurface is not NULL\n");
    }
    header_texture = SDL_CreateTextureFromSurface(app->renderer, header_textsurface);
	SDL_FreeSurface(header_textsurface);
    while(!app->quit) {
		while(SDL_PollEvent(&app->e)) {
			if(app->e.type == SDL_QUIT) {
				app->quit = true;
			} else if(app->e.type == SDL_TEXTINPUT ||
				app->e.type == SDL_KEYDOWN ||
				app->e.type == SDL_KEYUP) {
				inputbox_handle_events(app, &app->inputbox);
			}
		}
        app_getwindowsize(app);
        app_setwindowcolor(app, WINDOW_COLOR);
		SDL_SetRenderDrawColor(app->renderer, color_toparam(header_color));
		header.x = (app->win_width - header.w) / 2;
		SDL_RenderDrawRect(app->renderer, &header);
		SDL_RenderFillRect(app->renderer, &header);
		header_textcanvas.x = header.x + (header.w - header_textcanvas.w) / 2;
		header_textcanvas.y = header.y + (header.h - header_textcanvas.h) / 2;
        if(header_texture) {
            SDL_RenderCopy(app->renderer, header_texture, NULL, &header_textcanvas);
        }
		if(app->inputbox.enter_pressed) {
			message_list_add(&app->message_list, app->inputbox_font, app->font_size,
				app->inputbox.data,
				message_color,
				10,
				10,
				10,
				message_canvascolor
			);
			inputbox_clear(&app->inputbox);
		}
		int start_x = 10, start_y = header.y + header.h + 10;
		message_list_setstartpos(&app->message_list, start_x, start_y);
    	// Display our message
		message_list_display(app, &app->message_list);

		inputbox_init(&app->inputbox, app->inputbox_font, app->font_size,
			inputbox_textcolor,
			app->win_width - 40, 50, inputbox_color,
			2,
			50,
			(SDL_Color){0x00, 0xff, 0x00, 0xff},
			0.5,
			(SDL_Color){0x00, 0x00, 0xff, 0xff}
		);
		app->inputbox.canvas.x = (app->win_width - app->inputbox.canvas.w) / 2;
		app->inputbox.canvas.y = app->win_height - app->inputbox.canvas.h - 20;
		inputbox_display(app, &app->inputbox);
		SDL_RenderPresent(app->renderer);
	}
}

void app_destroy(app_t* app) {
	message_list_destroy(&app->message_list);
	inputbox_destroy(&app->inputbox);
	SDL_DestroyTexture(header_texture);
    SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
	TTF_CloseFont(app->font);
	TTF_CloseFont(app->inputbox_font);
	TTF_Quit();
	SDL_Quit();
}