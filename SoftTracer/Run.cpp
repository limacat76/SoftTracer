#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <thread>
#include <conio.h>
#include "Graphics/Graphics.h"
#include "Interface/Mailbox.h"
#include "Interface/Parameters.h"
#include "Interface/Target.h"
#include "Interface/WorkEngine.h"
#include "Renderers/Test.h"
#include "Renderers/Raytracer.h"
#include "Tutorial/Tutorial.h"
#include "Time.h"

void printDefines() {
#ifdef _DEBUG
	std::cout << "Additional defines go here \n";
#endif
}

void run_engine(const int &no_threads, const int &width, const int &height, WorkEngine &engine, pixel *&image, Target &target, const bool &log_threads, const bool &log_total) {
	make_picture_blank(image, width, height);
	
	std::vector<std::thread> threads(no_threads);
	std::vector<bool> allocated(no_threads);

	const int block_width = 64;
	const int block_height = 64;

	engine.initialize_scene(new Parameters(-1, no_threads, width, height, 0, block_width, 0, block_height));

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	if (log_total) {
		std::cout << "starting computation at " << limacat::take_my_time();
	}

	int frames = 0;
	const int hz = 1000 / 24;
	bool allClosed = false;
	std::chrono::time_point<std::chrono::system_clock> end;

	MailBox* mailbox = new MailBox(no_threads);
	Parameters* parameters = new Parameters[no_threads];

	bool quit = false;
	bool has_work = true;
	int allocated_width = 0;
	int allocated_height = 0;
	int allocated_threads = 0;

	int real_block_width = 0;
	int real_block_height = 0;

	while (has_work || !quit) {

		if (has_work && allocated_threads < no_threads) {
			for (int i = allocated_threads; i < no_threads; i++) {
				if (allocated[i] == false) {
					mailbox->to_main_finished_working[i] = false;

					real_block_width = block_width;
					if (allocated_width + real_block_width > width) {
						real_block_width = width - allocated_width;
					}

					real_block_height = block_height;
					if (allocated_height + real_block_height > height) {
						real_block_height = height - allocated_height;
					}

					parameters[i] = Parameters(i, no_threads, width, height, allocated_width, real_block_width, allocated_height, real_block_height);
					threads[i] = (std::thread(&WorkEngine::render, &engine, image, mailbox, (const void *)&parameters[i]));
					allocated[i] = true;
					allocated_threads++;
					allocated_width = allocated_width + block_width;
					if (allocated_width > width) {
						allocated_width = 0;
						allocated_height = allocated_height + block_height;
						if (allocated_height > height) {
							has_work = false;
						}
					}
				}
			}
		}

		target.loop(quit, frames, allClosed);
		if (!allClosed) {
			for (int i = 0; i < no_threads; i++) {
				bool has_finished = mailbox->to_main_finished_working[i];
				if (has_finished && allocated[i]) {
					threads[i].join();
					allocated[i] = false;
					allocated_threads--;
				}
			}
			bool found = true;
			for (int i = 0; found && i < no_threads; i++) {
				found = mailbox->to_main_finished_working[i];
			}
			if (found && (!has_work)) {
				allClosed = true;
				if (log_threads) {
					std::cout << "Threads stopped running, switching to blocking mode" << "\n";
				}
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
		for (int i = 0; i < no_threads; i++) {
			if (allocated[i]) {
				threads[i].join();
				allocated[i] = false;
				allocated_threads--;
			}
		}
	}

	if (log_threads) {
		for (int i = 0; i < no_threads; i++) {
			std::cout << "Thread " << i << " started at " << mailbox->to_main_start_working_time[i];
			std::cout << " finished at " << mailbox->to_main_finish_working_time[i] << "\n";
		}
	}

	if (log_total) {
		std::chrono::duration<double> elapsed_seconds = end - start;
		std::cout << "finished computation at " << limacat::take_my_time() << "elapsed time: " << elapsed_seconds.count() << "s\n";
		std::cout << "Rendered " << frames << " frames @" << frames / elapsed_seconds.count() << " fps \n";
	}
}

int main(int argc, char *argv[]) {
	printDefines();

//	const int width = 1920;
// const int height = 1080;
	const int width = 640;
	const int height = 480;

	pixel* image = new pixel[width * height];
	bool test_continue = false;
	// Headless target;
	SDLTarget target(image, width, height);
	target.set_auto_continue(test_continue);

	// Raytracer ta;
	// Attempt1::JBEngine ta;
	// JBikker::Engine engine;
	Bands engine;
	if (test_continue) {
		for (int ii = 0; ii < 10; ii++) {
			run_engine(8, width, height, engine, image, target, false, true);
		}
	} else {
		run_engine(8, width, height, engine, image, target, true, true);
	}

	target.stop();
	delete[] image;
	std::cin.clear();
	std::cout << "Press any key to exit \n";
#pragma warning( push ) 
#pragma warning( disable : 6031) 
	_getch();
#pragma warning( pop ) 

	return 0;
}

