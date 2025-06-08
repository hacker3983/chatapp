#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define color_toparam(color) color.r, color.g, color.b, color.a

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	TTF_Font* font = TTF_OpenFont("Roboto-Italic-VariableFont_wdth,wght.ttf",
		36);

	int quit = 0, win_width = 900, win_height = 600;
	SDL_Event e;
	SDL_Window* window = SDL_CreateWindow("Chat Application in C",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			win_width, win_height, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Color window_color = {255, 255, 255, 255};
	SDL_Rect inputbox = {
		.x = 0,
		.y = 0,
		.w = 100,
		.h = 50
	};
	SDL_Color inputbox_color = {0, 0, 255, 255};

	const char* header_text = "Chat Application in C";
	int header_w = 0, header_h = 0;
	TTF_SizeText(font, header_text, &header_w, &header_h);
	SDL_Color header_textcolor = {255, 255, 0, 255};
	SDL_Rect header_textcanvas = {
		.x = 0,
		.y = 0,
		.w = header_w,
		.h = header_h
	};
	SDL_Rect header = {
		.x = 0,
		.y = 0,
		.w = header_w + 40,
		.h = header_h + 40
	};
	SDL_Color header_color = {0};

	SDL_Surface* header_textsurface = TTF_RenderText_Blended(font, header_text,
		header_textcolor);
	SDL_Texture* header_texture = SDL_CreateTextureFromSurface(renderer, header_textsurface);
	SDL_FreeSurface(header_textsurface);
	while(!quit) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				quit = 1;
			}
		}
		SDL_GetWindowSize(window, &win_width, &win_height);
		SDL_SetRenderDrawColor(renderer, color_toparam(window_color));
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, color_toparam(header_color));
		header.x = (win_width - header.w) / 2;
		SDL_RenderDrawRect(renderer, &header);
		SDL_RenderFillRect(renderer, &header);
		header_textcanvas.x = header.x + (header.w - header_textcanvas.w) / 2;
		header_textcanvas.y = header.y + (header.h - header_textcanvas.h) / 2;
		SDL_RenderCopy(renderer, header_texture, NULL, &header_textcanvas);

		SDL_SetRenderDrawColor(renderer, color_toparam(inputbox_color));
		inputbox.w = win_width - 40;
		inputbox.x = (win_width - inputbox.w) / 2;
		inputbox.y = win_height - inputbox.h - 10;
		SDL_RenderDrawRect(renderer, &inputbox);
		SDL_RenderFillRect(renderer, &inputbox);
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	return 0;
}
