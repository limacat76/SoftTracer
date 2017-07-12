#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <thread>
#include <SDL.h>
#include <conio.h>
#include "Interface/CallableAlgorithm.h"
#include "Interface/Mailbox.h"
#include "Interface/Parameters.h"
#include "Graphics/Graphics.h"
#include "Renderers/Test.h"
#include "Renderers/Raytracer.h"
#include "Time.h"

std::vector<std::thread> threads;

int main(int argc, char *argv[]) {
	const int width = 1920;
	const int height = 1080;
	bool quit = false;

	SDL_Event event;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("SDLSoftTracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

	pixel* image = new pixel[width * height];
	make_picture_blank(image, width, height);

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cout << "starting computation at " << limacat::take_my_time();

	const int no_threads = 8;
	Raytracer ta;
	ta.initialize_scene();

	MailBox* mailbox = new MailBox(no_threads);

	Parameters* parameters = new Parameters[no_threads];

	for (int i = 0; i < no_threads; i++) {
		parameters[i] = Parameters(i, no_threads, width, height);
		threads.push_back(std::thread(&CallableAlgorithm::render, ta, image, mailbox, (const void *)&parameters[i]));
	}

	int frames = 0;
	const int hz = 1000 / 24;
	bool copy = true;
	bool allClosed = false;
	std::chrono::time_point<std::chrono::system_clock> end;
	int remainingFrames = 60;
	while (!quit) {
		if (copy) {
			SDL_UpdateTexture(texture, NULL, image, width * sizeof(uint32_t));
		}
		if (!allClosed) {
			bool found = true;
			for (int i = 0; found && i < no_threads; i++) {
				found = mailbox->to_main_finished_working[i];
			}
			if (found) {
				allClosed = true;
				std::cout << "Threads stopped running, switching to blocking mode" << "\n";
				end = std::chrono::system_clock::now();
			}
		}
		if (!allClosed || remainingFrames > 0) {
			if (allClosed) {
				remainingFrames--;
			}
			while (SDL_PollEvent(&event) != 0) {
				switch (event.type) {
				case SDL_QUIT:
					quit = true;
					mailbox->to_children_quit = true;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						quit = true;
						mailbox->to_children_quit = true;
					} else if (event.key.keysym.sym == SDLK_x) {
						copy = !copy;
					}
					break;
				}
			}
			frames++;
		} else {
			SDL_WaitEvent(&event);
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				} else if (event.key.keysym.sym == SDLK_x) {
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
	if (!allClosed) {
		std::cout << "Closed before" << "\n";
		end = std::chrono::system_clock::now();
	}
	for (int i = 0; i < no_threads; i++) {
		threads[i].join();
	}

	for (int i = 0; i < no_threads; i++) {
		std::cout << "Thread " << i << " started at " << mailbox->to_main_start_working_time[i];
		std::cout << " finished at " << mailbox->to_main_finish_working_time[i] << "\n";
	}

	delete[] image;
	delete[] parameters;
	delete mailbox;
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::cin.clear();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "finished computation at " << limacat::take_my_time() << "elapsed time: " << elapsed_seconds.count() << "s\n";
	std::cout << "Rendered " << frames << " frames @" << frames / elapsed_seconds.count() << " fps \n";

#pragma warning( push ) 
#pragma warning( disable : 6031) 
	_getch();
#pragma warning( pop ) 

	return 0;
}
