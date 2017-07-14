#include <iostream>
#include <random>
#include <limits>
#include <ctime>
#include <chrono>
#include <thread>
#include <conio.h>
#include "Time.h"
#include "Renderers/Test.h"
#include "Renderers/Raytracer.h"
#include "Interface/Mailbox.h"
#include "Interface/Target.h"

uint32_t* make_picture_noise(int width, int height) {
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<uint32_t> distr;
	const uint32_t all_alpha = 255 << 24;

	uint32_t* aPicture = new uint32_t[width * height];
	for (int row = 0; row < height; row++) {
		for (int column = 0; column < width; column++) {
			aPicture[row * width + column] = distr(eng) | all_alpha;
		}
	}
	return aPicture;
}

uint32_t* make_picture_test(int width, int height) {
	uint32_t* aPicture = new uint32_t[width * height];
	for (int row = 0; row < height; row++) {
		for (int column = 0; column < width; column++) {
			aPicture[row * width + column] =
				row % 255 << 16 +
				column % 255 << 8 +
				row + column % 255;
		}
	}
	return aPicture;
}

uint32_t* make_picture_blank(int width, int height) {
	uint32_t* aPicture = new uint32_t[width * height]{ 0 };
	return aPicture;
}

std::vector<std::thread> threads;

int main(int argc, char *argv[]) {
	const int width = 1920;
	const int height = 1080;
	bool quit = false;

	uint32_t* pixels = new uint32_t[width * height];
	uint32_t* image = make_picture_blank(width, height);
	// Headless target;
	SDLTarget target(pixels, width, height);

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
		threads.push_back(std::thread(&CallableAlgorithm::render, &ta, pixels, mailbox, (const void *)&parameters[i]));
	}

	int frames = 0;
	const int hz = 1000 / 24;
	bool allClosed = false;
	std::chrono::time_point<std::chrono::system_clock> end;

	while (!quit) {
		target.loop(quit, frames, allClosed);
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
		if (quit) {
			mailbox->to_children_quit = true;
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

	target.stop();
	delete[] pixels;
	delete[] image;
	std::cin.clear();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "finished computation at " << limacat::take_my_time() << "elapsed time: " << elapsed_seconds.count() << "s\n";
	std::cout << "Rendered " << frames << " frames @" << frames / elapsed_seconds.count() << " fps \n";
	_getch();
	
	return 0;
}
