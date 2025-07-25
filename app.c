#include "app.h"
#include "appconfig.h"
#include "message_list.h"
#include "app_strings.h"
#include "inputbox.h"
#include "chatsock.h"
#include <pthread.h>

bool received_message = false,
     recv_threadstarted = false;

void app_init() {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed to initialize SDL\n");
	}
	if(TTF_Init() < 0) {
		fprintf(stderr, "Failed to initialize SDL ttf\n");
	}
	chatsock_init();
}

void app_create(app_t* app) {
	app_init();
	app->server_sock = chatsock_create("127.0.0.1", PORT);
	if(chatsock_connect(&app->server_sock) < 0) {
		fprintf(stderr, "Failed to connect to the server\n");
	} else {
		printf("Connected to the server successfully!\n");
	}
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


bool app_rect_hover_horizontal(app_t* app, SDL_Rect rect) {
	return app->mouse_x <= rect.x + rect.w && app->mouse_x >= rect.x;
}

bool app_rect_hover_vertical(app_t* app, SDL_Rect rect) {
	return app->mouse_y <= rect.y + rect.h && app->mouse_y >= rect.y;
}

bool app_rect_hover(app_t* app, SDL_Rect rect) {
	return app_rect_hover_horizontal(app, rect)
		&& app_rect_hover_vertical(app, rect);	
}

void app_getwindowsize(app_t* app) {
	SDL_GetWindowSize(app->window, &app->win_width, &app->win_height);
}

void app_setwindowcolor(app_t* app, SDL_Color window_color) {
	SDL_SetRenderDrawColor(app->renderer, color_toparam(window_color));
	SDL_RenderClear(app->renderer);
}

void* app_recv_message(void* arg) {
	app_t* app = (app_t*)arg;
	size_t server_msglen = 0;
	chatsock_recv(app->server_sock, &server_msglen, sizeof(size_t));
	char* server_msg = calloc(server_msglen+1, sizeof(char));
	chatsock_recvall(app->server_sock, server_msg, server_msglen);
	received_message = true;
	return server_msg;
}

void app_run(app_t* app) {
	pthread_t recv_thread;
	while(!app->quit) {
		while(SDL_PollEvent(&app->e)) {
			if(app->e.type == SDL_QUIT) {
				app->quit = true;
			} else if(app->e.type == SDL_TEXTINPUT ||
				app->e.type == SDL_KEYDOWN ||
				app->e.type == SDL_KEYUP) {
				inputbox_handle_events(app, &app->inputbox);
			} else if(app->e.type == SDL_MOUSEMOTION) {
				app->mouse_x = app->e.motion.x,
				app->mouse_y = app->e.motion.y;
			} else if(app->e.type == SDL_MOUSEWHEEL) {
				if(app->e.wheel.y < 0) {
					app->scroll_type = APP_SCROLLDOWN;
					printf("Scrolling Down\n");
				} else if(app->e.wheel.y > 0) {
					app->scroll_type = APP_SCROLLUP;
					printf("Scroll Up\n");
				}
				app->scroll = true;
				printf("Detected scroll event!\n");
			}
		}
		app_getwindowsize(app);
		app_setwindowcolor(app, WINDOW_COLOR);

		if(!recv_threadstarted) {
			pthread_create(&recv_thread, NULL, &app_recv_message, app);
			recv_threadstarted = true;
		}
		if(recv_threadstarted && received_message) {
			char* message = NULL;
			pthread_join(recv_thread, (void**)&message);
			message_list_add(&app->message_list, app->inputbox_font, app->font_size,
				message,
				(SDL_Color){0xff, 0x00, 0x00, 0xff},
				10,
				10,
				10,
				message_canvascolor
			);
			free(message);
			recv_threadstarted = false;
			received_message = false;
		}
		if(app->inputbox.enter_pressed) {
			if(app->inputbox.data) {
				size_t message_size = strlen(app->inputbox.data);
				chatsock_send(app->server_sock, &message_size, sizeof(size_t));
				chatsock_send(app->server_sock, app->inputbox.data, message_size);
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
		}
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
		int start_x = 10, start_y = 1;
		SDL_Rect viewport = {
			.x = 0,
			.y = 0,
			.w = app->win_width,
			.h = app->inputbox.canvas.y - start_y
		};
		int scrollbar_w = 50,
		    scrollbar_h = 100;
		SDL_Color scrollbar_color = {0};
		SDL_Rect scrollbar = {
			.x = viewport.w - scrollbar_w,
			.y = viewport.y,
			.w = scrollbar_w,
			.h = scrollbar_h
		};
		message_list_setviewport(&app->message_list, viewport);
		message_list_setstartpos(&app->message_list, start_x, start_y);
		// Display our message
		message_list_display(app, &app->message_list);
		message_list_performscroll(app, &app->message_list);

		// Render the scroll bar
		if(app->message_list.messages) {
			double percentage = (double)app->message_list.render_pos /
				app->message_list.message_count;
			double percentage_of = (viewport.h - scrollbar_h)
				* percentage;
			scrollbar.y += (int)percentage_of;
		}
		SDL_SetRenderDrawColor(app->renderer, color_toparam(scrollbar_color));
		SDL_RenderDrawRect(app->renderer, &scrollbar);
		SDL_RenderFillRect(app->renderer, &scrollbar);
		inputbox_display(app, &app->inputbox);
		SDL_RenderPresent(app->renderer);
	}
}

void app_destroy(app_t* app) {
	chatsock_close(&app->server_sock);
	message_list_destroy(&app->message_list);
	inputbox_destroy(&app->inputbox);
	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
	TTF_CloseFont(app->font);
	TTF_CloseFont(app->inputbox_font);
	TTF_Quit();
	SDL_Quit();
}
