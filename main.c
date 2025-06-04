#include <SDL2/SDL.h>

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO);
	int quit = 0;
	SDL_Event e;
	SDL_Window* window = SDL_CreateWindow("Chat Application in C",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			900, 600, SDL_WINDOW_RESIZABLE);
	while(!quit) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				quit = 1;
			}
		}
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
