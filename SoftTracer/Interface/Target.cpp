#include "Target.h"

void Target::set_auto_continue(bool var) {
	auto_continue = var;
}

void Headless::loop(bool &quit, int &frames, bool &allClosed) {
	if (allClosed) {
		quit = true;
	}
}

void Headless::stop() {
	;
}


SDLTarget::SDLTarget(uint32_t* in_pixels, int in_width, int in_height) : pixels(in_pixels), width(in_width), height(in_height) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Raytracer with an SDL Output", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
}

SDLTarget::~SDLTarget() {

};

void SDLTarget::stop() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void SDLTarget::loop(bool &quit, int &frames, bool &allClosed) {
	if (copy) {
		SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(uint32_t));
	}

	if (!allClosed || remainingFrames > 0) {
		if (allClosed) {
			remainingFrames--;
			if (allClosed && auto_continue) {
				quit = true;
			}
		}
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				}
				else if (event.key.keysym.sym == SDLK_x) {
					copy = !copy;
				}
				break;
			}
		}
		frames++;
	}
	else {
		SDL_WaitEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				quit = true;
			}
			else if (event.key.keysym.sym == SDLK_x) {
				copy = !copy;
			}
			break;
		}
	}
	if (copy) {
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
}