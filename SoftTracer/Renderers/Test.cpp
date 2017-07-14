#include <random>
#include <limits>
#include "../Graphics/Graphics.h"
#include "../Interface/Mailbox.h"
#include "../Interface/Parameters.h"
#include "Test.h"

void Test::initialize_scene(const void* parameters) {
	// Test does not need to initialize a scene...
}

void Test::render(void* target, void* mailbox, const void* parameters) {
	pixel* image = (pixel*)target;
	MailBox* my_mailbox = (MailBox *)mailbox;
	const Parameters* myParameters = (const Parameters *)parameters;

	const int thread_no = myParameters->threadNumber;
	const int total_threads = myParameters->totalThreads;
	const int aSection = myParameters->height / total_threads;
	const int startY = thread_no * aSection;
	const int endY = thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;

	my_mailbox->work_started(thread_no);
	pixel* current_pixel = image + (startY * myParameters->width);

	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		for (int x = 0; !quit && x < myParameters->width; ++x, ++current_pixel) {
			pixel anUint = 255 << 8;
			anUint = (anUint + x%255) << 8;
			anUint = (anUint + y % 255) << 8;
			anUint = (anUint + (x + y) % 255);

			*current_pixel = anUint;
			quit = my_mailbox->to_children_quit;
		}
	}

	my_mailbox->work_done(thread_no);
}

void Bands::initialize_scene(const void* parameters) {
	// Bands does not need to initialize a scene...
}

void Bands::render(void* target, void* mailbox, const void* parameters) {
	pixel* image = (pixel*)target;
	MailBox* my_mailbox = (MailBox *)mailbox;
	const Parameters* myParameters = (const Parameters *)parameters;

	const int thread_no = myParameters->threadNumber;
	const int total_threads = myParameters->totalThreads;
	const int aSection = myParameters->height / total_threads;
	const int startY = thread_no * aSection;
	const int endY = thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;

	my_mailbox->work_started(thread_no);
	pixel* current_pixel = image + (startY * myParameters->width);

	const pixel luminosity = thread_no == total_threads - 1 ? 255 : (255 / total_threads) * thread_no;

	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		for (int x = 0; !quit && x < myParameters->width; ++x, ++current_pixel) {
			pixel anUint = 255 << 8;
			anUint = (anUint + luminosity) << 8;
			anUint = (anUint + luminosity) << 8;
			anUint = (anUint + luminosity);

			*current_pixel = anUint;
			quit = my_mailbox->to_children_quit;
		}
	}

	my_mailbox->work_done(thread_no);
}

void Noise::initialize_scene(const void* parameters) {
	// Noise does not need to initialize a scene...
}

void Noise::render(void* target, void* mailbox, const void* parameters) {
	pixel* image = (pixel*)target;
	MailBox* my_mailbox = (MailBox *)mailbox;
	const Parameters* myParameters = (const Parameters *)parameters;

	const int thread_no = myParameters->threadNumber;
	const int total_threads = myParameters->totalThreads;
	const int aSection = myParameters->height / total_threads;
	const int startY = thread_no * aSection;
	const int endY = thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;

	my_mailbox->work_started(thread_no);
	pixel* current_pixel = image + (startY * myParameters->width);

	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<pixel> distr;
	const pixel all_alpha = 255 << 24;

	// Trace rays
	bool quit = false;
	for (int y = startY; !quit && y < endY; ++y) {
		for (int x = 0; !quit && x < myParameters->width; ++x, ++current_pixel) {
			*current_pixel = distr(eng) | all_alpha;
			quit = my_mailbox->to_children_quit;
		}
	}

	my_mailbox->work_done(thread_no);
}