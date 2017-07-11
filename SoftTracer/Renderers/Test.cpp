#include <random>
#include <limits>
#include "Test.h"

void Test::initialize_scene() {
	// Test does not need to initialize a scene...
}

void Test::render(void* target, MailBox* mailbox, const void* parameters) {
	uint32_t* image = (uint32_t*)target;
	const Parameters* myParameters = (const Parameters *)parameters;

	const int thread_no = myParameters->threadNumber;
	const int total_threads = myParameters->totalThreads;
	const int aSection = myParameters->height / total_threads;
	const int startY = thread_no * aSection;
	const int endY = thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;

	mailbox->work_started(thread_no);
	uint32_t* pixel = image + (startY * myParameters->width);

	// Trace rays
	bool quit = false;
	for (unsigned y = startY; !quit && y < endY; ++y) {
		for (unsigned x = 0; !quit && x < myParameters->width; ++x, ++pixel) {
			uint32_t anUint = 255 << 8;
			anUint = (anUint + x%255) << 8;
			anUint = (anUint + y % 255) << 8;
			anUint = (anUint + (x + y) % 255);

			*pixel = anUint;
			quit = mailbox->to_children_quit;
		}
	}

	mailbox->work_done(thread_no);
}

void Bands::initialize_scene() {
	// Bands does not need to initialize a scene...
}

void Bands::render(void* target, MailBox* mailbox, const void* parameters) {
	uint32_t* image = (uint32_t*)target;
	const Parameters* myParameters = (const Parameters *)parameters;

	const int thread_no = myParameters->threadNumber;
	const int total_threads = myParameters->totalThreads;
	const int aSection = myParameters->height / total_threads;
	const int startY = thread_no * aSection;
	const int endY = thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;

	mailbox->work_started(thread_no);
	uint32_t* pixel = image + (startY * myParameters->width);

	const uint32_t luminosity = thread_no == total_threads - 1 ? 255 : (255 / total_threads) * thread_no;

	// Trace rays
	bool quit = false;
	for (unsigned y = startY; !quit && y < endY; ++y) {
		for (unsigned x = 0; !quit && x < myParameters->width; ++x, ++pixel) {
			uint32_t anUint = 255 << 8;
			anUint = (anUint + luminosity) << 8;
			anUint = (anUint + luminosity) << 8;
			anUint = (anUint + luminosity);

			*pixel = anUint;
			quit = mailbox->to_children_quit;
		}
	}

	mailbox->work_done(thread_no);
}

void Noise::initialize_scene() {
	// Noise does not need to initialize a scene...
}

void Noise::render(void* target, MailBox* mailbox, const void* parameters) {
	uint32_t* image = (uint32_t*)target;
	const Parameters* myParameters = (const Parameters *)parameters;

	const int thread_no = myParameters->threadNumber;
	const int total_threads = myParameters->totalThreads;
	const int aSection = myParameters->height / total_threads;
	const int startY = thread_no * aSection;
	const int endY = thread_no == total_threads - 1 ? myParameters->height : thread_no * aSection + aSection;

	mailbox->work_started(thread_no);
	uint32_t* pixel = image + (startY * myParameters->width);

	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<uint32_t> distr;
	const uint32_t all_alpha = 255 << 24;

	// Trace rays
	bool quit = false;
	for (unsigned y = startY; !quit && y < endY; ++y) {
		for (unsigned x = 0; !quit && x < myParameters->width; ++x, ++pixel) {
			uint32_t anUint = distr(eng) | all_alpha;
			*pixel = anUint;
			quit = mailbox->to_children_quit;
		}
	}

	mailbox->work_done(thread_no);
}