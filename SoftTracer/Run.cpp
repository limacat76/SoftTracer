#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <thread>
#include <conio.h>
#include "Interface/CallableAlgorithm.h"
#include "Interface/Mailbox.h"
#include "Interface/Parameters.h"
#include "Graphics/Graphics.h"
#include "Renderers/Test.h"
#include "Renderers/Raytracer.h"
#include "Interface/Mailbox.h"
#include "Interface/Target.h"
#include "Time.h"

std::vector<std::thread> threads;

void printDefines() {
#ifdef EIGEN_MPL2_ONLY
	std::cout << "compiled with EIGEN_MPL2_ONLY" << "\n";
#endif
}

int main(int argc, char *argv[]) {
	printDefines();

	const int width = 1920;
	const int height = 1080;
//	const int width = 320;
//	const int height = 240;

	bool quit = false;

	pixel* image = new pixel[width * height];
	make_picture_blank(image, width, height);
	// Headless target;
	SDLTarget target(image, width, height);

	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cout << "starting computation at " << limacat::take_my_time();

	const int no_threads = 8;
	// Raytracer ta;
	// Attempt1::JBEngine ta;
	JBRaytracer::JBEngine ta;
	ta.initialize_scene(new Parameters(-1, no_threads, width, height));

	MailBox* mailbox = new MailBox(no_threads);

	Parameters* parameters = new Parameters[no_threads];

	for (int i = 0; i < no_threads; i++) {
		parameters[i] = Parameters(i, no_threads, width, height);
		threads.push_back(std::thread(&CallableAlgorithm::render, &ta, image, mailbox, (const void *)&parameters[i]));
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
	delete[] image;
	std::cin.clear();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "finished computation at " << limacat::take_my_time() << "elapsed time: " << elapsed_seconds.count() << "s\n";
	std::cout << "Rendered " << frames << " frames @" << frames / elapsed_seconds.count() << " fps \n";

	std::cout << "Press any key to exit \n";
#pragma warning( push ) 
#pragma warning( disable : 6031) 
	_getch();
#pragma warning( pop ) 

	return 0;
}
