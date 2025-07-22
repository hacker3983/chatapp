#ifndef _APPCONFIG_H
#define _APPCONFIG_H
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define FONT_SIZE 36
#define PORT 7777
SDL_Color WINDOW_COLOR = {0x12, 0x12, 0x12, 0xff};
SDL_Rect inputbox = {
	.x = 0,
	.y = 0,
	.w = 100,
	.h = 50
};
SDL_Rect inputbox_textcanvas = {0};
SDL_Color inputbox_textcolor = {0xff, 0xff, 0xff, 0xff};
SDL_Texture* inputbox_texture = NULL;
SDL_Color inputbox_color = {0, 0, 0, 255};
char* message_buffer = NULL;

SDL_Texture* message_texture = NULL, *header_texture = NULL;
SDL_Color message_color = {0, 0xff, 0x0, 0xff},
		message_canvascolor = {0};
bool enter_pressed = false,
	display_message = false;
#endif
